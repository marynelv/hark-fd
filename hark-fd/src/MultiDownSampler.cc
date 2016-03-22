#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Matrix.h"
#include "Map.h"
#include <float.h>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace FD;

class MultiDownSampler;

DECLARE_NODE(MultiDownSampler)
/*Node
 *
 * @name MultiDownSampler
 * @category HARK:MISC
 * @description Down sampler for multi-channel input signal.
 *
 * @input_name INPUT
 * @input_type Matrix<float>
 * @input_description Input multi-channel signal.
 *
 * @output_name OUTPUT
 * @output_type Matrix<float>
 * @output_description Output multi-channel signal
 *
 * @parameter_name ADVANCE
 * @parameter_type int
 * @parameter_value 480
 * @parameter_description Size of data shift
 *
 * @parameter_name SAMPLING_RATE_IN
 * @parameter_type int
 * @parameter_value 48000
 * @parameter_description Sampling frequency of input signals.
 *
 * @parameter_name SAMPLING_RATE_OUT
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_description Sampling frequency of output signals.
 *
 * @parameter_name Wp
 * @parameter_type float
 * @parameter_value 0.28
 * @parameter_description Passband edge in normalized frequency. 
 *
 * @parameter_name Ws
 * @parameter_type float
 * @parameter_value 0.34
 * @parameter_description Stopband edge in normalized freqnency.
 *
 * @parameter_name As
 * @parameter_type float
 * @parameter_value 50
 * @parameter_description Minimum passband attenuation.
 *
 *
END*/
class MultiDownSampler : public BufferedNode {
    int inputID;
    int outputID;

    int inSampRate;
    int outSampRate;

    int adv;
    int itvl;

    double Wp;
    double Ws;
    double As;

    unsigned long n;
    double *lpf;

    Matrix<float> previous;
    
public:
    // コンストラクタ
    MultiDownSampler(string nodeName, ParameterSet params)
        : BufferedNode(nodeName, params)
    {
        
        // 入出力ポート生成
        inputID = addInput("INPUT");
        outputID = addOutput("OUTPUT");

        // パラメータ取得
        adv = dereference_cast<int>(parameters.get("ADVANCE"));

        inSampRate = dereference_cast<int>(parameters.get("SAMPLING_RATE_IN"));
        outSampRate = dereference_cast<int>(parameters.get("SAMPLING_RATE_OUT"));
        
        itvl = inSampRate / outSampRate;
        
        Wp = dereference_cast<float>(parameters.get("Wp"));
        Ws = dereference_cast<float>(parameters.get("Ws"));
        As = dereference_cast<float>(parameters.get("As"));

        // ローパスフィルタ生成
        lpf = lpf_fir_kaiser(&n, Wp*M_PI, Ws*M_PI, As);

        // エラーチェック
        if (Wp > Ws) {
            printf("Error in parameter-setting: 'Ws' must be larger than 'Wp'\n");
            exit(-1);
        }

        inOrder = true;
    }

    // 各イテレーションでの処理実体
    void calculate(int output_id, int count, Buffer &out)
    {
        RCPtr<Matrix<float> > input = getInput(inputID, count);
        RCPtr<Matrix<float> > output(new Matrix<float>(input->nrows(), input->ncols()/itvl));
        out[count] = output;

        //printf("%d %d %d\n", adv, input->ncols(), output->ncols());

        // for each source
        for (int i = 0; i < input->nrows(); i++) {
            // for each time-series
            for (int j = output->ncols() - adv/itvl; j < output->ncols(); j++) {
                (*output)(i, j) = 0;
                // 必要部分のみ出力
                for (int k = 0; k < n; k++) {
                    (*output)(i, j) += (*input)(i, itvl*j-k) * lpf[k];
                }
            }
        }

        // 
        if (count == 0) {
            previous.resize(output->nrows(), output->ncols());
            
            for (int i = 0; i < input->nrows(); i++) {
                for (int j = output->ncols() - adv/itvl; j < output->ncols(); j++) {
                    previous(i,j) = (*output)(i, j);
                }
            }
        }
        else {
            for (int i = 0; i < input->nrows(); i++) {
                for (int j = 0; j < output->ncols() - adv/itvl; j++) {
                    (*output)(i,j) = previous(i, j + adv/itvl);
                    previous(i,j) = previous(i, j + adv/itvl);
                }
                for (int j = output->ncols() - adv/itvl; j < output->ncols(); j++) {
                    previous(i,j) = (*output)(i, j);
                }
            }
        }
    }

    // 修正ベッセル関数
    double d_mbessel_I0(double x)
    {
        double ax, ans;
        double y;
        
        if ((ax = fabs(x)) < 3.75) {
            y = x / 3.75;
            y *= y;
            ans = 1.0 + y * (3.5156229 + y * (3.0899424 + y * (1.2067492
               + y * (0.2659732 + y * (0.360768E-1 + y * 0.45813E-2)))));
        }
        else {
            y = 3.75 / ax;
            ans = (exp(ax) / sqrt(ax)) * (0.39894228 + y * (0.1328592E-1
              + y * (0.225319E-2 + y * (-0.157565E-2 + y * (0.916281E-2
              + y * (-0.2057706E-1 + y * (0.2635537E-1 + y * (-0.1647633E-1
              + y * 0.392377E-2))))))));
        }
        
        return ans;
    }

    // カイザー窓関数生成
    void window_kaiser(double *w, unsigned long n, double beta)
    {
        unsigned long i;
        double x1, x2, y, a;
        
        n--;
        x2 = d_mbessel_I0(beta);
        
        for (i = 0; i <= n; i++) {
            y = (2.0 * (double)(i) / (double)(n)) - 1;
            y = 1.0 - y * y;
        
            if (y < 0.0) y = 0.0;
            else         y = sqrt(y);
            
            x1 = d_mbessel_I0(beta * y);
            w[i] = x1 / x2;
        }  
    }

    // 理想フィルタ生成
    double *lpf_ideal(unsigned long n, double Wc)
    {
        int i;
        double *h, x;
        
        h = (double *)calloc(sizeof(double), n);
        
        for (i = 0; i < n; i++) {
            x = (double)(i - n / 2.0);
            if (fabs(x) < DBL_EPSILON)
                x = Wc / M_PI;
            else
                h[i] = Wc / M_PI * (sin(Wc * x) / (Wc * x));
        }
        return h;
    }

    // カイザー窓に基づくローパスフィルタ生成
    double *lpf_fir_kaiser(unsigned long *n,
                           double Wp, double Ws, double As)
    {
        int i;
        double *lp, *h, beta, deltaW, deltaF;
        
        ////////////////////////////////////
        //           カイザー窓の計算
        ////////////////////////////////////
        // 変数計算
        deltaW = Ws - Wp; // 遷移域幅
        deltaF = deltaW / (2 * M_PI); // 規格化された遷移域幅
        
        // カイザーパラメータ計算
        if (As > 50) {
            beta = 0.1102 * (As - 8.7);
        }
        else if (21 < As && As <= 50){
            beta = 0.5842 * pow(As - 21.0, 0.4) + 0.07886 * (As - 21.0);
        }
        else {
            beta = 0;
        }
        
        // 次数計算
        (*n) = (unsigned long)ceil(((As - 7.95) / (14.36 * deltaF)));
                
        // メモリ確保
        lp = (double *)calloc(sizeof(double), *n);
        if (lp == NULL) return NULL;
        
        // カイザー窓
        window_kaiser(lp, *n, beta);
        
        ////////////////////////////////////
        //       理想インパルス応答との積
        ////////////////////////////////////
        h = lpf_ideal(*n, (Wp+Ws)/2.0);
        for (i = 0; i < *n; i++) {
            lp[i] *= h[i];
        }
        
        // 開放
        free(h);
        
        return lp;
    }
    
    IN_ORDER_NODE_SPEEDUP(MultiDownSampler)
    
};

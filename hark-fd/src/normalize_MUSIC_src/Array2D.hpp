/*
 * 2 dimensional array in a linear container
 * 6/10, 2013
 */
#ifndef _ARRAY2D_HPP_
#define _ARRAY2D_HPP_

#include <vector>
#include <algorithm>
#include <cassert>

template <typename T> class Array2D{
public:
	Array2D(){rows_ = 0; cols_ = 0;};
	Array2D(const size_t row, const size_t col){
		destructiveResize(row,col);
	}
	Array2D(const size_t row, const size_t col, const std::vector<T>& data):data_(data),rows_(row),cols_(col){
		assert(row >= 0 && col >= 0);
		assert(row*col == data.size());
		
		return;
	}
	virtual ~Array2D() {};
	
	inline const size_t rows() const { return rows_; }
	inline const size_t cols() const { return cols_; }
	inline const size_t size() const { return rows_*cols_; }
	
	// copy constructor
	Array2D(const Array2D<T>& src):data_(src.data_),rows_(src.rows_),cols_(src.cols_){}
	
	// assigment
	Array2D<T>& operator=(const Array2D<T>& rhs){
		data_ = rhs.data_;
		rows_ = rhs.rows_;
		cols_ = rhs.cols_;
		
		return *this;
	}
	// type cast
	template <typename T2> const Array2D<T2> cast_type() const {
		std::vector<T2> data(rows_*cols_);
		for(size_t n = 0, N = rows_*cols_; n < N; ++n){
			data[n] = static_cast<T2>(data_[n]);
		}
		
		Array2D<T2> obj(rows_, cols_, data);
		return obj;
	}
	
	// resize
	Array2D<T>& resize(const size_t new_row, const size_t new_col){
		assert(new_row >= 0 && new_col >= 0);
		const size_t new_size = new_row * new_col;
		
		std::vector<T> new_data(new_size);
		const size_t min_row = new_row < rows_ ? new_row : rows_;
		const size_t min_col = new_col < cols_ ? new_col : cols_;
		
		for(size_t r = 0; r < min_row; ++r){
			std::copy(data_.begin()+r*cols_, data_.begin()+r*cols_+min_col, new_data.begin()+r*new_col);
		}
		
		// update the data
		data_ = new_data;
		rows_ = new_row;
		cols_ = new_col;
		
		return *this;
	}
	Array2D<T>& destructiveResize(const size_t new_row, const size_t new_col){
		assert(new_row >= 0 && new_col >= 0);
		const size_t new_size = new_row * new_col;
		data_.assign(new_size,T(0));
		
		rows_ = new_row;
		cols_ = new_col;
		
		return *this;
	}
	
	// reshape
	void reshape_row_1st(const int row, const int col){
		assert(row >= 0 && col >= 0);
		assert(row*col == rows_*cols_);
		
		rows_ = row;
		cols_ = col;
		return;
	}
	void reshape(const int row, const int col){
		assert(row >= 0 && col >= 0);
		assert(row*col == rows_*cols_);
		
		std::vector<T> new_data(data_.size());
		for(int n = 0, N = data_.size(); n < N; ++n){
			// new place
			const int prev_row = n%rows_, prev_col = n/rows_;
			const int now_row = n%row, now_col = n/row;
			
			new_data[now_row*col+now_col] = data_[prev_row*cols_+prev_col];
		}
		
		// overwrite
		rows_ = row;
		cols_ = col;
		data_ = new_data;
		return;
	}
	
	// transpose
	Array2D<T> transpose(){
		const int row = cols_, col = rows_;
		
		std::vector<T> new_data(data_.size());
		for(int n = 0, N = data_.size(); n < N; ++n){
			// new place
			const int prev_row = n%rows_, prev_col = n/rows_;
			
			new_data[n] = data_[prev_row*cols_+prev_col];
		}
		
		// overwrite
		Array2D<T> obj(row, col, new_data);
		return obj;
	}
	
	// access
	inline const T& operator()(const int row, const int col) const { // read-only
		assert(row >= 0 && row < int(rows_));
		assert(col >= 0 && col < int(cols_));
		
		return data_[row*cols_+col];
	}
	inline T& operator()(const int row, const int col) { // writing
		assert(row >= 0 && row < int(rows_));
		assert(col >= 0 && col < int(cols_));
		
		return data_[row*cols_+col];
	}
	
	// block access
	const std::vector<T> col(const int index) const {
		assert(index >= 0 && index < cols_);
		
		std::vector<T> obj(rows_);
		for(size_t r = 0; r < rows_; ++r){
			obj[r] = this->operator()(r,index);
		}
		
		return obj;
	}
	const std::vector<T> row(const int index) const {
		assert(index >= 0 && index < int(rows_));
		
		std::vector<T> obj;
		obj.assign(data_.begin() + index*cols_, data_.begin()+(index+1)*cols_);
		
		return obj;
	}
	
	// static constructors
	static const Array2D<T> Zero(const int row, const int col) {
		assert(row >= 0 && col >= 0);
		Array2D<T> obj;
		obj.data_.assign(row*col, T(0));
		obj.rows_ = row;
		obj.cols_ = col;
		
		return obj;
	}
	static const Array2D<T> Constant(const int row, const int col, const T& val) {
		assert(row >= 0 && col >= 0);
		Array2D<T> obj;
		
		obj.data_.assign(row*col, val);
		obj.rows_ = row;
		obj.cols_ = col;
		
		return obj;
	}
	
	

private:
	std::vector<T> data_;
	size_t rows_, cols_;
};


template <typename T> void print_array(const Array2D<T>& src){
	const int rows = src.rows(), cols = src.cols();
	
	for(int i = 0; i < rows; ++i){
		for(int j = 0; j < cols; ++j){
			printf("x(%d,%d) = ",i,j);
			std::cout << src(i,j) << "\t" << std::flush;
		}
		std::cout << std::endl;
	}
}

#endif //_ARRAY2D_HPP_

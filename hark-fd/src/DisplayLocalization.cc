/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <iostream>
#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include <math.h>
#include "Source.h"
#include <string>
#include <gtk/gtk.h>
#include <assert.h>

struct gtk_str {
  GtkWidget *window;
  GtkWidget *drawingarea;
  GdkPixmap *pixmap;
  GdkRectangle plot_area;
  GdkGC *plot_gc[3];
  GdkGC *cursor_gc;
  int start_time;
  int time_length;
  int ymin;
  int ymax;
  bool is_alive;
};

gboolean delete_event_multi(GtkWidget* widget, GdkEventConfigure* event, gpointer data)
{
  gtk_str *prm = (gtk_str *)data;
  prm->window = NULL;
  return FALSE;  
  return TRUE;
}

gboolean configure_event_multi(GtkWidget* widget, GdkEventConfigure* event, gpointer data)
{
  gtk_str *prm = (gtk_str *)data;
  if (prm->pixmap) {
    gdk_pixmap_unref(prm->pixmap);
  }
    
  prm->pixmap = gdk_pixmap_new(widget->window,
			  widget->allocation.width,
			  widget->allocation.height,
			  -1);

  GdkColor red = {0, 0xffff, 0x0000, 0x0000};
  GdkColor green = {0, 0x0000, 0xffff, 0x0000};
  GdkColor blue = {0, 0x0000, 0x0000, 0xffff};
  gdk_colormap_alloc_color(gdk_colormap_get_system(), &red, FALSE, TRUE);
  gdk_colormap_alloc_color(gdk_colormap_get_system(), &green, FALSE, TRUE);
  gdk_colormap_alloc_color(gdk_colormap_get_system(), &blue, FALSE, TRUE);
  prm->plot_gc[0] = gdk_gc_new(prm->pixmap);
  prm->plot_gc[1] = gdk_gc_new(prm->pixmap);
  prm->plot_gc[2] = gdk_gc_new(prm->pixmap);
  gdk_gc_set_foreground(prm->plot_gc[0], &red);
  gdk_gc_set_foreground(prm->plot_gc[1], &green);
  gdk_gc_set_foreground(prm->plot_gc[2], &blue);
  gdk_gc_set_line_attributes(prm->plot_gc[0], 3, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);
  gdk_gc_set_line_attributes(prm->plot_gc[1], 3, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);
  gdk_gc_set_line_attributes(prm->plot_gc[2], 3, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);
  
  prm->cursor_gc = gdk_gc_new(prm->pixmap);
  gdk_gc_set_foreground(prm->cursor_gc, &red);
        
  gdk_draw_rectangle(prm->pixmap,
		     widget->style->white_gc,
		     TRUE, 0, 0,
		     widget->allocation.width,
		     widget->allocation.height);
    
  gdk_draw_rectangle(prm->pixmap,
		     widget->style->black_gc,
		     FALSE, 0, 0,
		     widget->allocation.width,
		     widget->allocation.height);
    
  prm->plot_area.x = widget->allocation.x + 30;
  prm->plot_area.y = widget->allocation.y + 10;
  prm->plot_area.width = widget->allocation.width - 40;
  prm->plot_area.height = widget->allocation.height - 30;

  if (prm->pixmap) {
      
    //GdkFont* font = gdk_font_load("-*-courier-medium-r-normal-*-10-*-*-*-*-*-iso8859-*");
    GdkFont* font = gdk_font_load("fixed");
      
    gdk_draw_rectangle(prm->pixmap,
		       prm->drawingarea->style->white_gc,
		       TRUE, prm->drawingarea->allocation.x, prm->drawingarea->allocation.y,
		       prm->drawingarea->allocation.width,
		       prm->drawingarea->allocation.height);
      
    gdk_draw_rectangle(prm->pixmap,
		       prm->drawingarea->style->black_gc,
		       FALSE, prm->plot_area.x, prm->plot_area.y,
		       prm->plot_area.width, prm->plot_area.height);
      
    char label[32];
    for (int angle = prm->ymin; angle <= prm->ymax; angle += 30) {
      snprintf(label, sizeof(label), "% 4d", angle);
      gdk_draw_string(prm->pixmap, font, prm->drawingarea->style->black_gc,
		      prm->plot_area.x - 28,
		      prm->plot_area.y + prm->plot_area.height * (prm->ymax - angle) / (prm->ymax - prm->ymin) + 5,
		      label);
    }
      
    for (int time = 0; time <= prm->time_length; time += 100) {
      snprintf(label, sizeof(label), "%d", prm->start_time + time);
      gdk_draw_string(prm->pixmap, font, prm->drawingarea->style->black_gc,
		      prm->plot_area.x + prm->plot_area.width * time / prm->time_length,
		      prm->plot_area.y + prm->plot_area.height + 10,
		      label);
    }
  }
        
  return TRUE;
}
  
gboolean expose_event_multi(GtkWidget* widget, GdkEventExpose* event, gpointer data)
{  
  gtk_str *prm = (gtk_str *)data;
  gdk_draw_pixmap(widget->window,
  		  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
  		  prm->pixmap,
  		  event->area.x, event->area.y,
  		  event->area.x, event->area.y,
  		  event->area.width, event->area.height);
  return TRUE;
}


using namespace std;
using namespace FD;

class DisplayLocalization;

DECLARE_NODE(DisplayLocalization);
/*Node
 *
 * @name DisplayLocalization
 * @category HARK:Localization
 * @description Display sound streams in the time-azimuth map.
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name OUTPUT
 * @output_type Vector<ObjectRef>
 * @output_description The same as input.
 *
 * @parameter_name WINDOW_NAME
 * @parameter_type string
 * @parameter_value Source Location
 * @parameter_description Window name of the time-azimuth map [default: Window name]
 *
 * @parameter_name WINDOW_LENGTH
 * @parameter_type int
 * @parameter_value 1000
 * @parameter_description Window length to show at the same time [sample]
 *
 * @parameter_name VERTICAL_RANGE
 * @parameter_type object
 * @parameter_value <Vector<int> -180 180>
 * @parameter_description Plot range of the vertical axis
 *
 * @parameter_name PLOT_TYPE
 * @parameter_type string
 * @parameter_value AZIMUTH
 * @parameter_list AZIMUTH:ELEVATION:X:Y:Z
 * @parameter_description Coordinate setting for the plotting
 * 
 END*/

class DisplayLocalization : public BufferedNode {
  int sourcesID;
  int outputID;
  string window_name;  
  string plot_type;
  gtk_str prms;
    
public:
  DisplayLocalization(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    sourcesID = addInput("SOURCES");
    outputID = addOutput("OUTPUT");

    prms.start_time = 0;
    prms.is_alive = true;
    prms.window = NULL;
    prms.drawingarea = NULL;
    prms.pixmap = NULL;

    int windowLength = dereference_cast<int>(parameters.get("WINDOW_LENGTH"));
    prms.time_length = windowLength;

    Vector<int> vertical_range;
    vertical_range = object_cast<Vector<int> >(parameters.get("VERTICAL_RANGE"));
    assert(vertical_range.size() == 2);
    assert(vertical_range[0] < vertical_range[1]);
    prms.ymin = vertical_range[0];
    prms.ymax = vertical_range[1];

    window_name = object_cast<String>(parameters.get("WINDOW_NAME"));
    plot_type = object_cast<string>(parameters.get("PLOT_TYPE"));

    inputsCache[sourcesID].lookBack = windowLength;
    inOrder = true;
  }

  void initialize(void)
  {
    this->BufferedNode::initialize();

    cout << "initialize\n";
    gdk_threads_enter();

    gtk_set_locale();
    gtk_init(0, 0);
    
    prms.is_alive = true;
    prms.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(prms.window), window_name.c_str());
    gtk_widget_set_usize(prms.window, 600, 300);
      
    prms.drawingarea = gtk_drawing_area_new();
    gtk_widget_show(prms.drawingarea);
    gtk_container_add(GTK_CONTAINER(prms.window), prms.drawingarea);
      
    gtk_signal_connect(GTK_OBJECT(prms.window), "delete_event",
		       (GtkSignalFunc)delete_event_multi, &prms);
    gtk_signal_connect(GTK_OBJECT(prms.drawingarea), "configure_event",
		       (GtkSignalFunc)configure_event_multi, &prms);
    gtk_signal_connect(GTK_OBJECT(prms.drawingarea), "expose_event",
		       (GtkSignalFunc)expose_event_multi, &prms);

    gtk_widget_show(prms.window);
    
    prms.start_time = 0;

    if (prms.pixmap) {

      //GdkFont* font = gdk_font_load("-*-courier-medium-r-normal-*-10-*-*-*-*-*-iso8859-*");
      GdkFont* font = gdk_font_load("fixed");
      
      gdk_draw_rectangle(prms.pixmap,
			 prms.drawingarea->style->white_gc,
			 TRUE, prms.drawingarea->allocation.x, prms.drawingarea->allocation.y,
			 prms.drawingarea->allocation.width,
			 prms.drawingarea->allocation.height);
      
      gdk_draw_rectangle(prms.pixmap,
			 prms.drawingarea->style->black_gc,
			 FALSE, prms.plot_area.x, prms.plot_area.y,
			 prms.plot_area.width, prms.plot_area.height);
      
      char label[32];
      for (int angle = prms.ymin; angle <= prms.ymax; angle += 30) {
	snprintf(label, sizeof(label), "% 4d", angle);
	gdk_draw_string(prms.pixmap, font, prms.drawingarea->style->black_gc,
			prms.plot_area.x - 28,
			prms.plot_area.y + prms.plot_area.height * (prms.ymax - angle) / (prms.ymax - prms.ymin) + 5,
			label);
      }
      
      for (int time = 0; time <= prms.time_length; time += 100) {
	snprintf(label, sizeof(label), "%d", prms.start_time + time);
	gdk_draw_string(prms.pixmap, font, prms.drawingarea->style->black_gc,
			prms.plot_area.x + prms.plot_area.width * time / prms.time_length,
			prms.plot_area.y + prms.plot_area.height + 10,
			label);
      }
    }

    GdkRectangle update_rect;
    update_rect.x = prms.drawingarea->allocation.x;
    update_rect.y = prms.drawingarea->allocation.y;
    update_rect.width = prms.drawingarea->allocation.width;
    update_rect.height = prms.drawingarea->allocation.height;
    gtk_widget_draw(prms.drawingarea, &update_rect);
    gdk_threads_leave();
  
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Vector<ObjectRef> > pSources = getInput(sourcesID, count);
    const Vector<ObjectRef>& sources = *pSources;

    out[count] = pSources;

    if (prms.is_alive && count % 100 == 0) {
      gdk_threads_enter();
      if (prms.window != NULL) {
	if (count > prms.start_time + prms.time_length) {
	  prms.start_time += prms.time_length / 2;
	}

	if (prms.pixmap) {
	  
	  //GdkFont* font = gdk_font_load("-*-courier-medium-r-normal-*-10-*-*-*-*-*-iso8859-*");
	  GdkFont* font = gdk_font_load("fixed");
	  
	  gdk_draw_rectangle(prms.pixmap,
			     prms.drawingarea->style->white_gc,
			     TRUE, prms.drawingarea->allocation.x, prms.drawingarea->allocation.y,
			     prms.drawingarea->allocation.width,
			     prms.drawingarea->allocation.height);
	  
	  gdk_draw_rectangle(prms.pixmap,
			     prms.drawingarea->style->black_gc,
			     FALSE, prms.plot_area.x, prms.plot_area.y,
			     prms.plot_area.width, prms.plot_area.height);
	  
	  char label[32];
	  for (int angle = prms.ymin; angle <= prms.ymax; angle += 30) {
	    snprintf(label, sizeof(label), "% 4d", angle);
	    gdk_draw_string(prms.pixmap, font, prms.drawingarea->style->black_gc,
			    prms.plot_area.x - 28,
			    prms.plot_area.y + prms.plot_area.height * (prms.ymax - angle) / (prms.ymax - prms.ymin) + 5,
			    label);
	  }
	  
	  for (int time = 0; time <= prms.time_length; time += 100) {
	    snprintf(label, sizeof(label), "%d", prms.start_time + time);
	    gdk_draw_string(prms.pixmap, font, prms.drawingarea->style->black_gc,
			    prms.plot_area.x + prms.plot_area.width * time / prms.time_length,
			    prms.plot_area.y + prms.plot_area.height + 10,
			    label);
	  }
	}

        plot(count,plot_type);
	gdk_draw_line(prms.pixmap, prms.cursor_gc,
		      prms.plot_area.x + (count - prms.start_time) * prms.plot_area.width / prms.time_length,
		      prms.plot_area.y,
		      prms.plot_area.x + (count - prms.start_time) * prms.plot_area.width / prms.time_length,
		      prms.plot_area.y + prms.plot_area.height);
	GdkRectangle update_rect;
	update_rect.x = prms.drawingarea->allocation.x;
	update_rect.y = prms.drawingarea->allocation.y;
	update_rect.width = prms.drawingarea->allocation.width;
	update_rect.height = prms.drawingarea->allocation.height;
	gtk_widget_draw(prms.drawingarea, &update_rect);
      }
      gtk_main_iteration_do(FALSE);
      gdk_flush();
      gdk_threads_leave();
    }

  }

  void plot(int count, string type) {
    int start_time_f = prms.start_time;
    int time_length_f = prms.time_length;
    map<int, vector<vector<GdkPoint> > > lines;
    int angle;
    GdkPoint pos;

    for (int t = start_time_f; t <= count; t++) {
      RCPtr<Vector<ObjectRef> > sources = getInput(sourcesID, t);
      for (int i = 0; i < sources->size(); i++) {
        RCPtr<Source> src = (*sources)[i];
	if(type == "AZIMUTH")
	  angle = (int)(180 / M_PI * atan2(src->x[1], src->x[0]));
	else if(type == "ELEVATION")
	  angle = (int)(180 / M_PI * atan2(src->x[2], sqrt(src->x[0] * src->x[0] + src->x[1] * src->x[1])));	  
	else if(type == "X")
	  angle = (int)src->x[0];	  
	else if(type == "Y")
	  angle = (int)src->x[1];	  
	else if(type == "Z")
	  angle = (int)src->x[2];	  
        pos.x = prms.plot_area.x + (t - prms.start_time) * prms.plot_area.width / prms.time_length;
        pos.y = prms.plot_area.y + (prms.ymax - angle) * prms.plot_area.height / (prms.ymax - prms.ymin);
	bool rflag = false;
	if (lines.find(src->id) != lines.end()){
	  if (lines[src->id][lines[src->id].size()-1].size() > 0){
	    GdkPoint prev_pos = lines[src->id][lines[src->id].size()-1][lines[src->id][lines[src->id].size()-1].size()-1];
	    if (abs(prev_pos.y - pos.y) >  (prms.ymax - prms.ymin) / 2){
	      rflag = true;
	    }
	  }
	  if (rflag) {
	    vector<GdkPoint> linetmp(0);
	    lines[src->id].push_back(linetmp);
	    lines[src->id][lines[src->id].size()-1].push_back(pos);
	  } else {	  
	    lines[src->id][lines[src->id].size()-1].push_back(pos);
	  }
	} else {
	  vector<GdkPoint> linetmp(0);
	  lines[src->id].push_back(linetmp);
	  lines[src->id][lines[src->id].size()-1].push_back(pos);	  
	}
      }
    }

    map<int, vector<vector<GdkPoint> > >::iterator it = lines.begin();
    for (; it != lines.end(); ++it) {
      for (int it2 = 0; it2 < it->second.size(); it2++) {
	gdk_draw_lines(prms.pixmap, prms.plot_gc[it->first % 3],&it->second[it2][0], it->second[it2].size());
      }
    }
  }

};

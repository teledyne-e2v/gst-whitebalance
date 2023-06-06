/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2022 Nicolas <<user@hostname.org>>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-whitebalance
 *
 * FIXME:Describe whitebalance here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! whitebalance ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "gstwhitebalance.h"
#include <stdbool.h>
void copy(const char* src,char *dest);

GST_DEBUG_CATEGORY_STATIC(gst_whitebalance_debug);
#define GST_CAT_DEFAULT gst_whitebalance_debug

/* Filter signals and args */
enum
{
    /* FILL ME */
    LAST_SIGNAL
};

enum
{
    PROP_0,
    PROP_BLUE,
    PROP_RED,
    PROP_GREEN,
    PROP_ROI1X,
    PROP_ROI1Y,
    PROP_ROI2X,
    PROP_ROI2Y,
    PROP_AUTOWHITEBALANCE
};

gboolean isFrozen = FALSE;

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS("ANY"));

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE("src",
                                                                  GST_PAD_SRC,
                                                                  GST_PAD_ALWAYS,
                                                                  GST_STATIC_CAPS("ANY"));

#define gst_whitebalance_parent_class parent_class
G_DEFINE_TYPE(Gstwhitebalance, gst_whitebalance, GST_TYPE_ELEMENT)

static void gst_whitebalance_set_property(GObject *object,
                                    guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_whitebalance_get_property(GObject *object,
                                    guint prop_id, GValue *value, GParamSpec *pspec);

static GstFlowReturn gst_whitebalance_chain(GstPad *pad,
                                      GstObject *parent, GstBuffer *buf);

static void gst_whitebalance_finalize(GObject *object);

void Auto_White_Balance(guint8* frame,int roi1x,int roi1y, int roi2x, int roi2y, int* red, int *green, int* blue);

/* GObject vmethod implementations */

/* initialize the whitebalance's class */
static void
gst_whitebalance_class_init(GstwhitebalanceClass *klass)
{
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;

    gobject_class = (GObjectClass *)klass;
    gstelement_class = (GstElementClass *)klass;

    gobject_class->set_property = gst_whitebalance_set_property;
    gobject_class->get_property = gst_whitebalance_get_property;
    gobject_class->finalize = gst_whitebalance_finalize;


    g_object_class_install_property(gobject_class, PROP_BLUE,
                                    g_param_spec_int("blue", "Blue",
                                                     "Control the value of blue gain (should be a string of 9 characters as the default value)",
                                                     0, 4096, 510, G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, PROP_AUTOWHITEBALANCE,
                                    g_param_spec_boolean("autowhitebalance", "Autowhitebalance",
                                                     "Do an auto white balance",
                                                     false, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_RED,
                                    g_param_spec_int("red", "Red",
                                                     "Control the value of red gain (should be a string of 9 characters as the default value)",
                                                     0, 4096, 566, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_GREEN,
                                    g_param_spec_int("green", "Green",
                                                     "Control the value of green gain (should be a string of 9 characters as the default value)",
                                                      0, 4096, 256, G_PARAM_READWRITE));

  g_object_class_install_property(gobject_class, PROP_ROI1X,
                                  g_param_spec_int("roi1x", "Roi1x", "Roi coordinates", 0, 1920, 0, G_PARAM_READWRITE));
  g_object_class_install_property(gobject_class, PROP_ROI1Y,
                                  g_param_spec_int("roi1y", "Roi1y", "Roi coordinates", 0, 1080, 0, G_PARAM_READWRITE));
  g_object_class_install_property(gobject_class, PROP_ROI2X,
                                  g_param_spec_int("roi2x", "Roi2x", "Roi coordinates", 0, 1920, 1920, G_PARAM_READWRITE));
  g_object_class_install_property(gobject_class, PROP_ROI2Y,
                                  g_param_spec_int("roi2y", "Roi2y", "Roi coordinates", 0, 1080, 1080, G_PARAM_READWRITE));

    gst_element_class_set_details_simple(gstelement_class,
                                         "whitebalance",
                                         "FIXME:Generic",
                                         "white balance the topaz sensor color",
                                         "Esisar-PI2022 <<user@hostname.org>>");

    gst_element_class_add_pad_template(gstelement_class,
                                       gst_static_pad_template_get(&src_factory));
    gst_element_class_add_pad_template(gstelement_class,
                                       gst_static_pad_template_get(&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_whitebalance_init(Gstwhitebalance *whitebalance)
{
    whitebalance->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
    gst_pad_set_chain_function(whitebalance->sinkpad,
                               GST_DEBUG_FUNCPTR(gst_whitebalance_chain));
    GST_PAD_SET_PROXY_CAPS(whitebalance->sinkpad);
    gst_element_add_pad(GST_ELEMENT(whitebalance), whitebalance->sinkpad);

    whitebalance->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
    GST_PAD_SET_PROXY_CAPS(whitebalance->srcpad);
    gst_element_add_pad(GST_ELEMENT(whitebalance), whitebalance->srcpad);
    whitebalance->frame = NULL;
	
    whitebalance->autowhitebalance = false;
    whitebalance->blue = 510;
    whitebalance->red = 566;
    whitebalance->green = 256;
    //whitebalance->blue = 256;
    //whitebalance->red = 256;

  whitebalance->ROI1x = 0;
  whitebalance->ROI1y = 0;
  whitebalance->ROI2x = 1920;
  whitebalance->ROI2y = 1080;




}


void copy(const char* src,char *dest)
{
	int i = 0;

	while(src[i])
	{
		dest[i]=src[i];
		i++;    
	}
	dest[i]='\0';
	
}

static void
gst_whitebalance_set_property(GObject *object, guint prop_id,
                        const GValue *value, GParamSpec *pspec)
{
    Gstwhitebalance *whitebalance = GST_WHITEBALANCE(object);

    switch (prop_id)
    {
    case PROP_BLUE:
        whitebalance->blue = g_value_get_int(value);
	break;
    case PROP_RED:
        whitebalance->red = g_value_get_int(value);
	break;
    case PROP_GREEN:
        whitebalance->green = g_value_get_int(value);
	break;
  case PROP_ROI1X:
    whitebalance->ROI1x = g_value_get_int(value);
    break;
  case PROP_ROI1Y:
    whitebalance->ROI1y = g_value_get_int(value);
    break;
  case PROP_ROI2X:
    whitebalance->ROI2x = g_value_get_int(value);
    break;
  case PROP_AUTOWHITEBALANCE:
    whitebalance->autowhitebalance = g_value_get_boolean(value);
    break;
  case PROP_ROI2Y:
    whitebalance->ROI2y = g_value_get_int(value);
    break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
gst_whitebalance_get_property(GObject *object, guint prop_id,
                        GValue *value, GParamSpec *pspec)
{
    Gstwhitebalance *whitebalance = GST_WHITEBALANCE(object);

    switch (prop_id)
    {
    case PROP_BLUE:
        g_value_set_int(value, whitebalance->blue);
        break;
  case PROP_ROI1X:
    g_value_set_int(value, whitebalance->ROI1x);
    break;
  case PROP_ROI1Y:
    g_value_set_int(value, whitebalance->ROI1y);
    break;
  case PROP_ROI2X:
    g_value_set_int(value, whitebalance->ROI2x);
    break;
  case PROP_ROI2Y:
    g_value_set_int(value, whitebalance->ROI2y);
    break;
  case PROP_AUTOWHITEBALANCE:
    g_value_set_boolean(value, whitebalance->autowhitebalance);
    break;
    case PROP_RED:
        g_value_set_int(value, whitebalance->red);
        break;
    case PROP_GREEN:
        g_value_set_int(value, whitebalance->green);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/* chain function
 * this function does the actual processing
 */


void Auto_White_Balance(guint8* frame,int roi1x,int roi1y, int roi2x, int roi2y, int* red, int *green, int* blue)
{
		long int green_sum=0;
	long int blue_sum=0;
	long int red_sum=0;

        roi1x-=roi1x%2;
	roi1y-=roi1y%2;
	roi2x+=roi2x%2;
	roi2y+=roi2y%2;
	
	for(int x = roi1x; x < roi2x; x+=2)
	{
		for(int y = roi1y; y < roi2y; y+=2)
		{
			if(frame[x+1+y*1920] != 0 || frame[x+1+y*1920] != 255) // to avoid the saturated pixels
			{
				red_sum += frame[x+y*1920];
				green_sum += frame[x+1+y*1920];
				green_sum += frame[x+(y+1)*1920];
				blue_sum += frame[x+1+(y+1)*1920];
			}
		}	
	}
	green_sum=green_sum/2; // there is two the green pixels for one blue and one red
	*blue =(int) (((green_sum / ((float)blue_sum)) * ((*blue / ((float) (*green)))))*256);
	*red =(int) (((green_sum / ((float)red_sum)) * ((*red / ((float) (*green)))))*256);
	g_print("red : %d\n", *red);
	g_print("blue : %d\n", *blue);
}


static GstFlowReturn
gst_whitebalance_chain(GstPad *pad, GstObject *parent, GstBuffer *buf)
{

    GstMapInfo map;
    Gstwhitebalance *whitebalance = GST_WHITEBALANCE(parent);

    if(proc_once==0)
    {
    if(system("i2ctransfer -f -y 6 w3@0x10 0x04 0xa2 0xd0")==-1)
    {
	  printf("system call failed\n");
    }
	proc_once=1;
    }
    gst_buffer_map(buf, &map, GST_MAP_READ);
    
    if(previous_blue != whitebalance->blue && !whitebalance->autowhitebalance)
    {
	char cmd[100];
	if(sprintf(cmd,"i2ctransfer -f -y 6 w3@0x10 0x1E 0x%02x 0x%02x",(int) whitebalance->blue/ 256, whitebalance->blue % 256)==-1)
	{	
		g_print("failed to copy cmd\n");
	}
	else
	{
		printf("%s",cmd);
		if(system(cmd)==-1)	
	    	{
		    	g_print("system call failed\n");
	    	}
	}

    
    	previous_blue=whitebalance->blue;
    }

    if(previous_red != whitebalance->red && !whitebalance->autowhitebalance)
    {
	char cmd[100];
     	if(sprintf(cmd,"i2ctransfer -f -y 6 w3@0x10 0x20 0x%02x 0x%02x", (int) whitebalance->red/ 256, whitebalance->red % 256)==-1)
	{
		g_print("failed to copy cmd\n");
	}
	else
	{
		if(system(cmd)==-1)	
    		{
	    		g_print("system call failed\n");
    		}
	}
	previous_red=whitebalance->red;
    }

    if(previous_green != whitebalance->green && !whitebalance->autowhitebalance)
    {
	char cmd[100];
        if(sprintf(cmd,"i2ctransfer -f -y 6 w3@0x10 0x1F 0x%02x 0x%02x", (int) whitebalance->green/ 256, whitebalance->green % 256)==-1)
	{
		g_print("failed to copy cmd\n");
	}
	else
	{
		if(system(cmd)==-1)	
    		{
	    		g_print("system call failed\n");
    		}
	}
	previous_green=whitebalance->green;
    }

    if(whitebalance->autowhitebalance)
    {
	Auto_White_Balance(map.data, whitebalance->ROI1x, whitebalance->ROI1y, whitebalance->ROI2x, whitebalance->ROI2y,&(previous_red),&(previous_green),&(previous_blue));
	whitebalance->red = previous_red;
	whitebalance->blue = previous_blue;
	previous_red = 256;
	previous_blue = 256;
	whitebalance->autowhitebalance=false;
    }
    
    frame ++;
    /* just push out the incoming buffer without touching it */
    gst_buffer_unmap(buf,&map);
    return gst_pad_push(whitebalance->srcpad, buf);
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
whitebalance_init(GstPlugin *whitebalance)
{
    /* debug category for whitebalanceing log messages
     *
     * exchange the string 'Template whitebalance' with your description
     */
    GST_DEBUG_CATEGORY_INIT(gst_whitebalance_debug, "whitebalance",
                            0, "Template whitebalance");

    return gst_element_register(whitebalance, "whitebalance", GST_RANK_NONE,
                                GST_TYPE_WHITEBALANCE);
}

static void gst_whitebalance_finalize(GObject *object)
{
    Gstwhitebalance *whitebalance = GST_WHITEBALANCE(object);

    free(whitebalance->frame);
}
/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstwhitebalance"
#endif

/* gstreamer looks for this structure to register whitebalances
 *
 * exchange the string 'Template whitebalance' with your whitebalance description
 */

#ifdef HAVE_CONFIG_H

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    whitebalance,
    "Template whitebalance",
    whitebalance_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/")
#else
GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    whitebalance,
    "Template whitebalance",
    whitebalance_init,
    "Unknown",
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/")
#endif

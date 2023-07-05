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

#ifndef __WHITEBALANCE_H__
#define __WHITEBALANCE_H__

#include <gst/gst.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_WHITEBALANCE \
    (gst_whitebalance_get_type())
#define GST_WHITEBALANCE(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_WHITEBALANCE, Gstwhitebalance))
#define GST_WHITEBALANCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_WHITEBALANCE, GstwhitebalanceClass))
#define GST_IS_WHITEBALANCE(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_WHITEBALANCE))
#define GST_IS_WHITEBALANCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_WHITEBALANCE))

typedef struct _Gstwhitebalance Gstwhitebalance;
typedef struct _GstwhitebalanceClass GstwhitebalanceClass;

int previous_blue = 256;
int previous_red = 256;
int previous_green = 256;
int frame = 0;
int proc_once = 0;
struct _Gstwhitebalance
{
    GstElement element;

    GstPad *sinkpad, *srcpad;

    gboolean autowhitebalance;
    gint blue;
    gint red;
    gint green;
    guint8 *frame;
    gint ROI1x;
    gint ROI1y;
    gint ROI2x;
    gint ROI2y;
    gsize frameSize;
};

struct _GstwhitebalanceClass
{
    GstElementClass parent_class;
};

GType gst_whitebalance_get_type(void);

G_END_DECLS

#endif /* __WHITEBALANCE_H__ */

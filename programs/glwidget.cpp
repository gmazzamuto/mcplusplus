/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtOpenGL>
#include <cmath>

#include <iostream>

#include "glwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    scale = 5e-3;

    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
    displayedAxisLength = 0;
    xTrans = 0;
    yTrans = 0;
    zTrans = 0;
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const

{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setScale(MCfloat value) {
    if(value<1e-4)
        return;
    scale = value;
    updateGL();
}

void GLWidget::initializeGL()
{
    qglClearColor(qtPurple.dark());

    // Set the drawing color to green
    glColor3f( 0.0f, 1.0f, 0.0f );

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    //    glEnable(GL_LIGHTING);
    //    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    //    static GLfloat lightPosition[4] = { 10, 5.0, 7.0, 1.0 };
    //    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    //    GLfloat mat_specular[] = {0.3, 0.3, 0.3, 1.0};
    //    GLfloat mat_shininess[] = { 10.0 };
    //    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    //    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    //    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    //    glEnable(GL_COLOR_MATERIAL);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw XYZ reference axes
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f(displayedOriginPos[0],displayedOriginPos[1],displayedOriginPos[2]);
    glVertex3f(displayedOriginPos[0]+displayedAxisLength,displayedOriginPos[1],displayedOriginPos[2]);

    glColor3f(0,1,0);
    glVertex3f(displayedOriginPos[0],displayedOriginPos[1],displayedOriginPos[2]);
    glVertex3f(displayedOriginPos[0],displayedOriginPos[1]+displayedAxisLength,displayedOriginPos[2]);

    glColor3f(0,0,1);
    glVertex3f(displayedOriginPos[0],displayedOriginPos[1],displayedOriginPos[2]);
    glVertex3f(displayedOriginPos[0],displayedOriginPos[1],displayedOriginPos[2]+displayedAxisLength);
    glEnd();

    paint_GL_impl();

    glLoadIdentity();
    glTranslatef(xTrans / 16.0, yTrans / 16.0, -10);
    glScalef(scale,scale,scale);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMax(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-1, +1, -1, +1, 4, 15);
#else
    glOrtho(-1, +1, -1, +1, 4, 15);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    } else if(event->buttons() & Qt::MiddleButton) {
        setScale(scale + scale/30*dy);
    }
    lastPos = event->pos();
}

void GLWidget::keyPressEvent(QKeyEvent * event)
{
    switch(event->key()) {
    case Qt::Key_Up:
        yTrans-=0.5;
        break;
    case Qt::Key_Down:
        yTrans+=0.5;
        break;
    case Qt::Key_Right:
        xTrans+=0.5;
        break;
    case Qt::Key_Left:
        xTrans-=0.5;
        break;

    default:
        return;
    }

    event->accept();
    updateGL();
}

void GLWidget::setDisplayedOriginPos(float *pos) {
    memcpy(displayedOriginPos,pos,3*sizeof(MCfloat));
}

void GLWidget::setDisplayedOriginPos(float x, float y, float z)
{
    displayedOriginPos[0] = x;
    displayedOriginPos[1] = y;
    displayedOriginPos[2] = z;
}

void GLWidget::setDisplayedAxisLength(float length) {
    displayedAxisLength = length;
}

void GLWidget::paint_GL_impl() {

}

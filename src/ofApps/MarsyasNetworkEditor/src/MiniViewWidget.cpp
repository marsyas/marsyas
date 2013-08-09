//
//  MiniViewWidget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 2/28/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "MiniViewWidget.h"
#include "ofMain.h"
#include "GraphicalParameters.h"

using namespace Marsyas;


MiniViewWidget::MiniViewWidget(GraphicalEnvironment* env) : Widget(0, 0, 0, 0, env)
{
  x_ = 10;
  y_ = 10;
  height_ = MINIVIEW_HEIGHT - 20;
  width_ = height_*env_->networkHeight_*ofGetWidth()*(ofGetWidth() - 20)/(env_->networkWidth_*(ofGetHeight() - MINIVIEW_HEIGHT - 10)*(MINIVIEW_HEIGHT - 20));
  miniDragLockDown_ = false;
  miniDragLockUp_ = false;

  move_ = false;
}



void MiniViewWidget::update()
{
  lockZoomScaleXY();
  if(move_) {
    cout<<endl<<"aqui 1";
    bool reach = true;
    double dx = targetX_ - x_;
    if(abs(dx) > 4.0) {
      x_ += dx * EASING;
      reach = false;
    }

    double dy = targetY_ - y_;
    if(abs(dy) > 4.0) {
      y_ += dy * EASING;
      reach = false;
    }
    if(reach) {
      move_ = false;
    }
  }
}

void MiniViewWidget::updatePosition(int x0, int y0) {

}


void MiniViewWidget::moveTo(int tx, int ty) {
  cout<<endl<<"aqui 2";
  targetX_ = tx*env_->miniViewXscale_ + 10 - width_*0.5;
  targetY_ = ty*env_->miniViewYscale_ + 10 - height_*0.5;
  move_ = true;

}

void MiniViewWidget::draw()
{

  ofSetColor(255, 230, 0);
  ofNoFill();
  ofRect(x_, y_, width_, height_);
  ofFill();
  ofSetColor(255, 230, 0, 50);
  ofRect(x_, y_, width_, height_);
  ofSetColor(255, 255, 0, 255);
  ofRect(x_ + width_ - MINIVIEW_ZOOM_HANDLER, y_ + height_ - MINIVIEW_ZOOM_HANDLER, MINIVIEW_ZOOM_HANDLER, MINIVIEW_ZOOM_HANDLER);
  ofRect(x_ , y_ , MINIVIEW_ZOOM_HANDLER, MINIVIEW_ZOOM_HANDLER);

}



void MiniViewWidget::focusFromMarControlDiagram(int x0, int y0) {

}


bool MiniViewWidget::mouseOver() {
  isMouseOver_ = false;
  if(x_ <= env_->mouse_->x && (x_ + width_) >= env_->mouse_->x)
  {
    if(y_ <= env_->mouse_->y && (y_ + height_) >= env_->mouse_->y)
    {
      isMouseOver_ = true;
      return true;
    }
  }

  return false;
}


bool MiniViewWidget::mousePressed() {

  if(mouseOver())
  {
    if(env_->mouse_->click == 0)
    {

      dragLock_ = true;


      if(env_->mouse_->x >= x_ + width_ - MINIVIEW_ZOOM_HANDLER) {
        if(env_->mouse_->x <= x_ + width_) {
          if(env_->mouse_->y >= y_ + height_ - MINIVIEW_ZOOM_HANDLER) {
            if(env_->mouse_->y <= y_ + height_) {
              miniDragLockDown_ = true;
            }
          }
        }
      }

      if(env_->mouse_->x >= x_) {
        if(env_->mouse_->x <= x_ + MINIVIEW_ZOOM_HANDLER) {
          if(env_->mouse_->y >= y_) {
            if(env_->mouse_->y <= y_ + MINIVIEW_ZOOM_HANDLER) {
              miniDragLockUp_ = true;
            }
          }
        }
      }


      if(!miniDragLockDown_) {
        clickPointOffsetX_ = env_->mouse_->x - x_;
        clickPointOffsetY_ = env_->mouse_->y - y_;
      }

      if(!miniDragLockUp_) {
        clickPointOffsetX_ = env_->mouse_->x - x_;
        clickPointOffsetY_ = env_->mouse_->y - y_;
      }

      return true;
    }
  }
  else {
    if(env_->mouse_->click == 0)
    {
      x_ = env_->mouse_->x - width_*0.5;
      y_ = env_->mouse_->y - height_*0.5;

      if(x_ < 0) {
        x_ = 0;
      }


      if((x_ + width_) > ofGetWidth()) {
        x_ = ofGetWidth() - width_;
      }

      if(y_ < 0) {
        y_ = 0;
      }

      if((y_ + height_) > MINIVIEW_HEIGHT) {
        y_ = MINIVIEW_HEIGHT - height_;
      }

      env_->xOffset_ = -x_;
      env_->yOffset_ = -y_ + MINIVIEW_HEIGHT;
      return true;
    }

  }
  return false;
}

bool MiniViewWidget::mouseDragged()
{

  if(dragLock_) {
    if(env_->mouse_->drag == 0)
    {

      if(miniDragLockDown_) {

        /*
        width_ = env_->mouse_->x - x_;
        if(width_ < 30){
            width_ = 30;
        }
        if((x_ + width_) > ofGetWidth()){
            width_ = ofGetWidth() - x_;
        }
        */
        //height_ = width_*env_->networkHeight_/(env_->networkWidth_);


        height_ = env_->mouse_->y - y_;
        if(height_ < 30) {
          height_ = 30;
        }
        if((y_ + height_) > MINIVIEW_HEIGHT) {
          height_ = MINIVIEW_HEIGHT - y_;
        }

        //width_ =  height_*ofGetWidth()/(ofGetHeight() - MINIVIEW_HEIGHT);
        //width_ = height_*env_->networkWidth_/(env_->networkHeight_);
        width_ = height_*env_->networkHeight_*ofGetWidth()*(ofGetWidth() - 20)/(env_->networkWidth_*(ofGetHeight() - MINIVIEW_HEIGHT - 10)*(MINIVIEW_HEIGHT - 20));
      }
      else if(miniDragLockUp_) {
        int lockHeight = y_ + height_;
        int lockWidth = x_ + width_;

        y_ = env_->mouse_->y;
        if(y_ < 0) {
          y_ = 0;
        }
        if((y_ + 30) > MINIVIEW_HEIGHT) {
          y_ = MINIVIEW_HEIGHT - 30;
        }


        x_ = env_->mouse_->x;

        height_ = lockHeight - y_;
        width_ = lockWidth - x_;

        width_ = height_*env_->networkHeight_*ofGetWidth()*(ofGetWidth() - 20)/(env_->networkWidth_*(ofGetHeight() - MINIVIEW_HEIGHT - 10)*(MINIVIEW_HEIGHT - 20));
      }
      else {

        x_ = env_->mouse_->x - clickPointOffsetX_;
        y_ = env_->mouse_->y - clickPointOffsetY_;

        if(x_ < 0) {
          x_ = 0;
        }


        if((x_ + width_) > ofGetWidth()) {
          x_ = ofGetWidth() - width_;
        }

        if(y_ < 0) {
          y_ = 0;
        }

        if((y_ + height_) > MINIVIEW_HEIGHT) {
          y_ = MINIVIEW_HEIGHT - height_;
        }

        env_->xOffset_ = -x_;
        env_->yOffset_ =  - y_ + MINIVIEW_HEIGHT - 10;
      }
      return true;
    }
  }
  return false;
}

void MiniViewWidget::lockZoomScaleXY() {
  width_ = height_*env_->networkHeight_*ofGetWidth()*(ofGetWidth() - 20)/(env_->networkWidth_*(ofGetHeight() - MINIVIEW_HEIGHT - 10)*(MINIVIEW_HEIGHT - 20));
}

void MiniViewWidget::windowResized() {

}

bool MiniViewWidget::mouseReleased() {
  Widget::mouseReleased();
  miniDragLockDown_ = false;
  miniDragLockUp_ = false;
}

void MiniViewWidget::debugger() {
  cout<<endl<<"mVf x = "<<x_;
}

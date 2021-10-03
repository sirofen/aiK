//https://stackoverflow.com/a/68141638/14073801
#pragma once

#include <QToolButton>
#include <QApplication>
#include <QDebug>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class CollapseButton : public QToolButton {
public:
  CollapseButton(QWidget *parent) : QToolButton(parent), content_(nullptr) {
    setCheckable(true);
    setStyleSheet("background:none");
    setIconSize(QSize(8, 8));
    setFont(QApplication::font());
    connect(this, &QToolButton::toggled, [=](bool checked) {
      setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::LeftArrow);
      content_ != nullptr && checked ? showContent() : hideContent();
    });
  }

  void setText(const QString &text) {
    QToolButton::setText(" " + text);
  }

  void setContent(QWidget *root, QWidget *parent, QWidget *content) {
      assert(content != nullptr || parent != nullptr || root != nullptr);
      auto height_diff = content->baseSize().height() + 14;
      m_root_widget = root;                                  /* "size" */
      auto root_widget_anim = new QPropertyAnimation(m_root_widget, "maximumHeight");
      root_widget_anim->setStartValue(m_root_widget->height());
      root_widget_anim->setEasingCurve(QEasingCurve::OutQuad);
      root_widget_anim->setDuration(500);
      root_widget_anim->setEndValue(m_root_widget->height() + height_diff);
      animator_.addAnimation(root_widget_anim);

//      auto parent_frame_margins_anim = new QPropertyAnimation(parent, "layoutBottomMargin");
//      parent_frame_margins_anim->setStartValue(0);
//      parent_frame_margins_anim->setEasingCurve(QEasingCurve::InOutQuad);
//      parent_frame_margins_anim->setDuration(300);
//      parent_frame_margins_anim->setEndValue(9);
//      animator_.addAnimation(root_widget_anim);

      content_ = content;
      auto animation_ = new QPropertyAnimation(content_, "maximumHeight"); // QObject with auto delete
      animation_->setStartValue(0);
      animation_->setEasingCurve(QEasingCurve::InOutQuad);
      animation_->setDuration(805);
      animation_->setEndValue(height_diff);
      animator_.addAnimation(animation_);
      if (!isChecked()) {
          content->setMaximumHeight(0);
      }
  }

  void hideContent() {
    animator_.setDirection(QAbstractAnimation::Backward);
    animator_.start();
    m_root_widget->setProperty("maximumHeight", m_root_widget->minimumHeight());
    //m_parent->setMaximumHeight(1);
  }

  void showContent() {
    animator_.setDirection(QAbstractAnimation::Forward);
    animator_.start();
  }

private:
  QWidget *content_;
  QWidget *m_root_widget;
  QParallelAnimationGroup animator_;
};

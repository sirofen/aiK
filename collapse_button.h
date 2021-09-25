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

  void setContent(QWidget *parent, QWidget *content) {
    assert(content != nullptr || parent != nullptr);
    auto height_diff = content->geometry().height() + 10;
    m_parent = parent;                                  /* "size" */
    auto parent_anim = new QPropertyAnimation(m_parent, "maximumHeight");
    parent_anim->setStartValue(m_parent->height());
    parent_anim->setEasingCurve(QEasingCurve::InOutQuad);
    parent_anim->setDuration(300);
    parent_anim->setEndValue(m_parent->height() + height_diff);
    animator_.addAnimation(parent_anim);

    content_ = content;
    auto animation_ = new QPropertyAnimation(content_, "maximumHeight"); // QObject with auto delete
    animation_->setStartValue(0);
    animation_->setEasingCurve(QEasingCurve::InOutQuad);
    animation_->setDuration(300);
    animation_->setEndValue(height_diff);
    animator_.addAnimation(animation_);
    if (!isChecked()) {
      content->setMaximumHeight(0);
    }
  }

  void hideContent() {
    animator_.setDirection(QAbstractAnimation::Backward);
    animator_.start();
    //m_parent->setMaximumHeight(1);
  }

  void showContent() {
    animator_.setDirection(QAbstractAnimation::Forward);
    animator_.start();
  }

private:
  QWidget *content_;
  QWidget *m_parent;
  QParallelAnimationGroup animator_;
};

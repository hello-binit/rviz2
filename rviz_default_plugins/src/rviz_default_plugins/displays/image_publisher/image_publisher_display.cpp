#include "rviz_default_plugins/displays/image_publisher/image_publisher_display.hpp"

#include <OgreRenderWindow.h>
#include <OgreViewport.h>

#include "rviz_common/display_context.hpp"
#include "rviz_common/properties/string_property.hpp"
#include "rviz_common/render_panel.hpp"
#include "rviz_rendering/render_window.hpp"

#include <pluginlib/class_list_macros.hpp>

namespace rviz_default_plugins
{
namespace displays
{

ImagePublisherDisplay::ImagePublisherDisplay()
: publishing_(false)
{
  topic_property_ = new rviz_common::properties::StringProperty(
    "Topic", "/rviz/render_image",
    "The image topic to publish to.",
    this, SLOT(updateTopic()));
}

ImagePublisherDisplay::~ImagePublisherDisplay()
{
  if (context_ && context_->getRenderPanel() && context_->getRenderPanel()->getRenderWindow()) {
    rviz_rendering::RenderWindowOgreAdapter::removeListener(
      context_->getRenderPanel()->getRenderWindow(), this);
  }
}

void ImagePublisherDisplay::onInitialize()
{
  node_ = context_->getRosNodeAbstraction().lock()->get_raw_node();
  updateTopic();
  
  if (context_->getRenderPanel() && context_->getRenderPanel()->getRenderWindow()) {
    rviz_rendering::RenderWindowOgreAdapter::addListener(
      context_->getRenderPanel()->getRenderWindow(), this);
  }
}

void ImagePublisherDisplay::update(float /*wall_dt*/, float /*ros_dt*/)
{
}

void ImagePublisherDisplay::reset()
{
  Display::reset();
}

void ImagePublisherDisplay::onEnable()
{
  publishing_ = true;
}

void ImagePublisherDisplay::onDisable()
{
  publishing_ = false;
}

void ImagePublisherDisplay::updateTopic()
{
  publisher_.reset();
  if (!topic_property_->getString().isEmpty()) {
    publisher_ = node_->create_publisher<sensor_msgs::msg::Image>(
      topic_property_->getStdString(), 10);
  }
}

void ImagePublisherDisplay::preRenderTargetUpdate(const Ogre::RenderTargetEvent & /*evt*/)
{
}

void ImagePublisherDisplay::postRenderTargetUpdate(const Ogre::RenderTargetEvent & evt)
{
  if (!publishing_ || !publisher_) {
    return;
  }

  // Get source
  Ogre::RenderTarget * renderTarget = evt.source;
  if (!renderTarget) {
    return;
  }

  unsigned int width = renderTarget->getWidth();
  unsigned int height = renderTarget->getHeight();

  auto msg = std::make_unique<sensor_msgs::msg::Image>();
  msg->header.stamp = node_->now();
  msg->header.frame_id = "rviz_render_window";
  msg->height = height;
  msg->width = width;
  msg->encoding = "rgb8";
  msg->is_bigendian = 0;
  msg->step = width * 3;
  msg->data.resize(height * width * 3);

  Ogre::PixelBox pb(width, height, 1, Ogre::PF_BYTE_RGB, msg->data.data());
  renderTarget->copyContentsToMemory(pb, Ogre::RenderTarget::FB_AUTO);

  publisher_->publish(std::move(msg));
}

}  // namespace displays
}  // namespace rviz_default_plugins

PLUGINLIB_EXPORT_CLASS(rviz_default_plugins::displays::ImagePublisherDisplay, rviz_common::Display)

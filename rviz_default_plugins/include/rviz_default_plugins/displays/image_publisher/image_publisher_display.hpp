#ifndef RVIZ_DEFAULT_PLUGINS__DISPLAYS__IMAGE_PUBLISHER__IMAGE_PUBLISHER_DISPLAY_HPP_
#define RVIZ_DEFAULT_PLUGINS__DISPLAYS__IMAGE_PUBLISHER__IMAGE_PUBLISHER_DISPLAY_HPP_

#include <memory>

#include <OgreRenderTargetListener.h>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"

#include "rviz_common/display.hpp"

namespace rviz_common
{
namespace properties
{
class StringProperty;
class IntProperty;
}  // namespace properties
}  // namespace rviz_common

namespace rviz_default_plugins
{
namespace displays
{

class ImagePublisherDisplay : public rviz_common::Display, public Ogre::RenderTargetListener
{
  Q_OBJECT

public:
  ImagePublisherDisplay();
  ~ImagePublisherDisplay() override;

  void onInitialize() override;
  void update(float wall_dt, float ros_dt) override;
  void reset() override;

  void preRenderTargetUpdate(const Ogre::RenderTargetEvent & evt) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent & evt) override;

protected Q_SLOTS:
  void updateTopic();

protected:
  void onEnable() override;
  void onDisable() override;

private:
  void setupPublisher();

  rviz_common::properties::StringProperty * topic_property_;

  rclcpp::Node::SharedPtr node_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;

  bool publishing_;
};

}  // namespace displays
}  // namespace rviz_default_plugins

#endif  // RVIZ_DEFAULT_PLUGINS__DISPLAYS__IMAGE_PUBLISHER__IMAGE_PUBLISHER_DISPLAY_HPP_

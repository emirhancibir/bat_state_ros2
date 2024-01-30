#include <chrono>
#include <functional>
#include <memory>


#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"
#include "power_msg/msg/battery_state.hpp"

using namespace std::chrono_literals;

class PublisherNode : public rclcpp::Node
{
    public:
        PublisherNode() : Node("bat_state_publisher") , current_capacity_(10.0)
        {
            declare_parameter("x", 0); // default param is 0
            declare_parameter("y", 0.0);
            declare_parameter("z", 0);
            declare_parameter("w", 0.0);

            get_parameter("x", x_time);
            get_parameter("y", y);
            get_parameter("z", z_time);
            get_parameter("w", w);
            std::cout<< "Battery Node Started !! "<< std::endl;
            
            publisher_ = this->create_publisher<power_msg::msg::BatteryState>("bat_topic", 10); 
            timer_ = this->create_wall_timer(500ms, std::bind(&PublisherNode::callback, this));

            charge_subscriber_ = this->create_subscription<std_msgs::msg::Bool>("is_charging_topic", 10,std::bind(&PublisherNode::chargeCallback, this, std::placeholders::_1));
        }



    private:
        void callback()
        {  // max cell of voltage 4.2 min 3.2
            power_msg::msg::BatteryState message = power_msg::msg::BatteryState();                  
        
            message.is_charging = charge_selector;
            message.name = "Profuse 2S ";
            message.charge_level = 100.0;
            message.total_capacity = 100.0;
            message.supply_voltage = 3.7;
            message.charger_voltage = 7.4;

            if (message.is_charging == true && current_capacity_ < message.total_capacity)
            {
                // CHARGE
                current_capacity_ += y;
                message.current_capacity = current_capacity_;
                message.battery_voltage = 6.4 + (current_capacity_ / 100)*(8.4-6.4);

                std::cout << "Current capacity : "<< message.current_capacity << std::endl;

                if (current_capacity_ >= message.total_capacity * 0.98)
                {
                    // full charged
                    std::cout << "Full charged" << std::endl;
                    message.current_capacity = message.total_capacity;

                    publisher_ -> publish(message);
                }

                if(current_capacity_ <= message.total_capacity )
                {
                    publisher_ -> publish(message);
                }

                rclcpp::sleep_for(std::chrono::milliseconds(x_time*1000));
            }

            if (message.is_charging == false && current_capacity_ > 1.0)
            {
                // DISCHARGE
                current_capacity_ -= w;
                message.current_capacity = current_capacity_;
                message.battery_voltage = 6.4 + (current_capacity_ / 100)*(8.4-6.4);
                std::cout << "Current capacity : " << message.current_capacity << std::endl;
                publisher_ -> publish(message);

                rclcpp::sleep_for(std::chrono::milliseconds(z_time*1000));
            }

            
            
        }
        
        void chargeCallback(const std_msgs::msg::Bool::SharedPtr msg)
        {
            charge_selector = msg->data;
        }
        
        rclcpp::Publisher<power_msg::msg::BatteryState>::SharedPtr publisher_;
        rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr charge_subscriber_; 

        rclcpp::TimerBase::SharedPtr timer_;

        int x_time, z_time;
        double y,w;
        double current_capacity_;
        bool charge_selector;

};


int main(int argc, char * argv[] )
{

    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<PublisherNode>());

    rclcpp::shutdown();
    return 0;
}
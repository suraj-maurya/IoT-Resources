# MQTT-Message Queuing Telemetry Transport   ([MQTT.org](https://mqtt.org/))

MQTT is the most commonly used messaging protocol for the Internet of Things (IoT). MQTT stands for MQ Telemetry Transport. The protocol is a set of rules that defines how IoT devices can publish and subscribe to data over the Internet. MQTT is used for messaging and data exchange between IoT and industrial IoT (IIoT) devices, such as embedded devices, sensors, industrial PLCs, etc. 

![Archtectire](https://media.geeksforgeeks.org/wp-content/uploads/20200627234409/mqtt.png)

> [!NOTE]
> - It requires minimal resources since it is lightweight and efficient
> - Supports bi-directional messaging between device and cloud
> - Can scale to millions of connected devices
> - Supports reliable message delivery through 3 QoS levels
> - Works well over unreliable networks
> -  Security enabled, so it works with TLS and common authentication protocols

### Difference between MQTT and HTTP protocols 
MQTT and HTTP are both network protocols but serve different communication patterns and use cases. MQTT is optimized for efficient, lightweight messaging in IoT and telemetry applications, while HTTP is widely used for client-server communication in web-based applications

#### Communication Pattern:
- MQTT: MQTT follows a publish-subscribe messaging pattern. It allows devices to publish messages to specific topics, and other devices can subscribe to those topics to receive messages. This asynchronous communication pattern is well-suited for real-time data streaming and efficient resource usage.
- HTTP: HTTP follows a request-response model. A client sends a request to a server, and the server responds with the requested data. This synchronous communication pattern is typical for web applications and works well for scenarios where immediate responses are required.

#### Message Size:
- MQTT: MQTT is designed to be lightweight and efficient, making it suitable for use cases with constrained resources such as bandwidth or battery power. It has smaller overhead compared to HTTP, which is advantageous for IoT devices and applications.
- HTTP: HTTP headers and other metadata can add significant overhead to each request and response, especially for small payloads. While technologies like HTTP/2 aim to mitigate this issue, HTTP typically involves larger message sizes compared to MQTT.

#### Connection Persistence:
- MQTT: MQTT supports persistent connections, where clients can establish long-lived connections with the MQTT broker. This allows for efficient message delivery and reduces the overhead of establishing connections for each message.
- HTTP: HTTP operates on a stateless model, where each request is independent of previous requests. While techniques like cookies and session management can introduce statefulness to HTTP applications, the protocol itself does not inherently support persistent connections.

#### Usage:
- MQTT: MQTT is commonly used in IoT (Internet of Things) applications, where devices need to communicate efficiently over unreliable or constrained networks. It's suitable for scenarios such as sensor data collection, device control, and telemetry.
- HTTP: HTTP is ubiquitous on the World Wide Web and is the foundation of modern web applications. It's used for browsing websites, accessing APIs, transferring files, and various other applications where client-server communication is required.

#### Reliability:
- MQTT: MQTT offers configurable levels of Quality of Service (QoS) to ensure message delivery. It supports at most once (QoS 0), at least once (QoS 1), and exactly once (QoS 2) delivery semantics, allowing applications to trade off between message delivery guarantees and network overhead.
- HTTP: HTTP typically relies on TCP for reliable data delivery. While HTTP itself does not provide built-in mechanisms for message acknowledgment and retransmission, applications can implement retry mechanisms and error handling to improve reliability.


## [Introducing the MQTT Protocol – MQTT Essentials: Part 1](https://www.hivemq.com/blog/mqtt-essentials-part-1-introducing-mqtt/)

### What is MQTT
MQTT is a Client Server publish/subscribe messaging transport protocol. It is lightweight, open, simple, and designed so as to be easy to implement. These characteristics make it ideal for use in many situations, including constrained environments such as for communication in Machine to Machine (M2M) and Internet of Things (IoT) contexts where a small code footprint is required and/or network bandwidth is at a premium.

> [!NOTE]
> MQTT was created with a focus on sending small amounts of data over unreliable networks with limited bandwidth and connectivity

> [!NOTE]
> MQTT uses a binary message format for communication between clients and brokers. This is in contrast to other protocols that use text-based formats, such as HTTP or SMTP.

### Origin
In 1999, Andy Stanford-Clark of IBM and Arlen Nipper of Arcom (now Cirrus Link) developed the MQTT protocol to enable minimal battery loss and bandwidth usage when connecting with oil pipelines via satellite. The inventors specified several requirements for the protocol, including:
- Simple implementation
- Quality of Service data delivery
- Lightweight and bandwidth-efficient
- Data agnostic
- Continuous session awareness

![History](https://www.hivemq.com/sb-assets/f/243938/600x1200/dd67e69190/when-was-mqtt-discovered.webp)

### The Role of OASIS in Standardizing MQTT(Organization for the Advancement of Structured Information Standards)

In 2014, OASIS announced that it would take over the standardization of MQTT, with the goal of making it an open and vendor-neutral protocol. Founded in 1993 as a non-profit, OASIS (Organization for the Advancement of Structured Information Standards) is an international consortium that develops open standards for the Internet and related technologies.
The standardization process took around one year, and on October 29, 2014, MQTT became an officially approved OASIS standard.

In March 2019, OASIS ratified the new MQTT 5 specification. This version introduced new features to MQTT that are required for IoT applications deployed on cloud platforms, and cases that require more reliability and error handling to implement mission-critical messaging. To learn more about MQTT 5, please check out our [MQTT 5](https://www.hivemq.com/mqtt/mqtt-5/) Essentials article series.


## Exploring MQTT: Topics, Subscriptions, QoS, Persistent Messaging, and More

### MQTT's Messaging Model: How It Works and Why It Matters for IoT & IIoT
MQTT’s messaging model is based on topics and subscriptions. Topics are strings that messages are published to and subscribed to. Topics are hierarchical and can contain multiple levels separated by slashes, like a file path as shown below.

```
myhome/kitchen/smartdishwasher
```

Subscriptions are used to specify which topics a client is interested in receiving messages from.
When a client subscribes to a topic, it is essentially telling the broker that it is interested in receiving messages published to that topic. The broker then keeps track of the subscription and forwards any messages published to that topic to the subscribed client.

![Image](https://www.hivemq.com/sb-assets/f/243938/850x350/31f395bf44/request-response-smart-door.gif)

> [!NOTE]
> In addition to topics and subscriptions, MQTT also supports wildcards, which can be used to subscribe to multiple topics that match a certain pattern. The two types of wildcards are the single-level wildcard (+), which matches a single level in a topic, and the multi-level wildcard (#), which matches all levels after the specified level in a topic.

### Understanding MQTT Quality of Service (QoS) Levels for IoT Applications
MQTT supports three levels of Quality of Service (QoS): QoS 0, QoS 1, and QoS 2.

*QoS 0*: This level provides “at most once” delivery, where messages are sent without confirmation and may be lost. This is the lowest level of QoS and is typically used in situations where message loss is acceptable or where the message is not critical. For example, QoS 0 might be appropriate for sending sensor data where occasional data loss would not significantly impact the overall results.

*QoS 1:* This level provides “at least once” delivery, where messages are confirmed and re-sent if necessary. With QoS 1, the publisher sends the message to the broker and waits for confirmation before proceeding. If the broker does not respond within a set time, the publisher re-sends the message. This level of QoS is typically used in situations where message loss is unacceptable, but message duplication is tolerable. For example, QoS 1 might be appropriate for sending command messages to devices, where a missed command could have serious consequences, but duplicated commands would not.

*QoS 2:* This level provides “exactly once” delivery, where messages are confirmed and re-sent until they are received exactly once by the subscriber. QoS 2 is the highest level of QoS and is typically used in situations where message loss or duplication is completely unacceptable. With QoS 2, the publisher and broker engage in a two-step confirmation process, where the broker stores the message until it has been received and acknowledged by the subscriber. This level of QoS is typically used for critical messages such as financial transactions or emergency alerts.

> [!IMPORTANT]
> It’s important to note that higher QoS levels typically require more resources and can result in increased latency and network traffic. As a result, it’s important to choose the appropriate QoS level based on the specific needs of your application. 
> It’s important to note that higher QoS levels typically require more resources and can result in increased latency and network traffic. As a result, it’s important to choose the appropriate QoS level based on the specific needs of your application. 


## Understanding MQTT Message Persistence for Reliable IoT Communication 
 
Message persistence is an important feature in MQTT. It ensures messages are not lost in the event of a network or server failure. In MQTT, message persistence is achieved by storing messages on the server until they are delivered to the subscriber.

MQTT provides three types of message persistence options:

*Non-persistent:* This is the default option in MQTT. In this mode, messages are not stored on the server and are lost if the server or network fails. This mode is suitable for situations where messages are not critical and can be easily regenerated.

*Queued persistent:* In this mode, messages are stored on the server until they are delivered to the subscriber. If the subscriber is not available, messages are queued until the subscriber reconnects. Queued persistence is useful when the subscriber is not always connected to the network, or if the subscriber needs to receive all messages, even if they are sent when the subscriber is offline.

*Persistent with acknowledgment:* This mode provides the highest level of message persistence. In this mode, messages are stored on the server until they are delivered to the subscriber, and the subscriber must acknowledge receipt of the message. If the subscriber does not acknowledge receipt, the message is re-sent until the subscriber acknowledges receipt. This mode is useful when it is critical to ensure that messages are received and processed by the subscriber.

> [!NOTE]
> To configure message persistence in MQTT, the broker software used for handling MQTT connections must support the chosen persistence option. The configuration can be done through the broker’s configuration files or through its web interface.
> It is important to note that message persistence comes with a trade-off in terms of performance and storage. The more persistent the messages, the more storage and processing resources are required by the broker. Therefore, it is important to choose the appropriate persistence level based on the specific requirements of the application.

## MQTT Security: Protecting Your IoT Devices from Cyber Attacks

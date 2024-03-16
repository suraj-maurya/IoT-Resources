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


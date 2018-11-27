import paho.mqtt.client as mqtt
import time



# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("$SYS/#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    

    if msg.topic == "BPM":
        BPM = int(msg.payload)
        print("BPM: ", BPM)
        
        if BPM < 60 or BPM > 100:
            print("\nAlarme BPM: ", BPM,"\n")
            client.publish("BPMFlag", 1)
        else:
            client.publish("BPMFlag", 0)
        
    if msg.topic == "SPO2":
        SPO2 = int(msg.payload)
        print("SPO2: ", SPO2)
        
        if SPO2 < 90:
            print("\nAlarme SPO2: ", SPO2,"\n")
            client.publish("SPO2Flag", 1)
        else:
            client.publish("SPO2Flag", 0)
        
    if msg.topic == "airFlow":
        global airFlowSum
        global airFlowCount
        airFlow = int(msg.payload)
        print("airFlow: ", airFlow)   
        print("airFlowCount: ", airFlowCount)           
        print("airFlowSum: ", airFlowSum)  
        airFlow = int(msg.payload) 
        airFlowSum = airFlowSum + airFlow
        airFlowCount += 1
        
        
        if airFlowCount >= 30:
            airFlowMean = airFlowSum/airFlowCount
        
            if airFlowMean < 10:
                print("\nAlarme airFlow: ", airFlowMean,"\n")
                client.publish("airFlowFlag", 1)
            else:
                client.publish("airFlowFlag", 0)

            airFlowSum = 0
            airFlowCount = 0
        

print("cliente")
client = mqtt.Client("NóProcessador")
client.on_connect = on_connect
client.on_message = on_message

print("conexão")
client.connect("192.168.0.4", 883, 60)
print("conexão CONCLUÍDA")


airFlowSum = 0
airFlowCount = 0

client.subscribe("BPM", 0)
client.subscribe("SPO2", 0)
client.subscribe("airFlow", 0)

client.loop_start()

while True:
    time.sleep(100)
    
 
    
    
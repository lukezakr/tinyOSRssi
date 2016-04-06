import math

class NetworkMap:
    
    def __init__(self, distanceFile):
        self.links = []
        self.nodes = []
        self.parseDistances(distanceFile)
        self.dmin = [0,1000000000]
        
    def parseDistances(self, distanceFile):
        nodefile = open(distanceFile, "r")
        for line in nodefile.readlines():
            line = line.rstrip()
            link = line.split(",")
            self.links.append(link)
        nodefile.close()
        #print(self.links)
        return
    
    def findOrigin(self):
        sendingNodes = []
        targetNodes = []
        sumOfDistances = {}
        for i in range(0,len(self.links)):
            tempLink = self.links[i]
            sendingNodes.append([tempLink[0], float(tempLink[2])])
            targetNodes.append([tempLink[1], float(tempLink[2])])
        for i in range(0,len(sendingNodes)):
            halfLink = sendingNodes[i]
            if halfLink[0] in sumOfDistances.keys():
                tempSum = sumOfDistances[halfLink[0]]
                sumOfDistances[halfLink[0]] = halfLink[1] + tempSum
            else:
                sumOfDistances[halfLink[0]] = halfLink[1]
                
        for i in range(0,len(targetNodes)):
            halfLink = targetNodes[i]
            if halfLink[0] in sumOfDistances.keys():
                tempSum = sumOfDistances[halfLink[0]]
                sumOfDistances[halfLink[0]] = halfLink[1] + tempSum
            else:
                sumOfDistances[halfLink[0]] = halfLink[1]
                
        for key,val in sumOfDistances.items():
            if (self.dmin[1] > val):
                self.dmin[1] = val
                self.dmin[0] = key
        
        print(self.dmin)
        return
    




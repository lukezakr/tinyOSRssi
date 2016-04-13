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
        
        #print(self.dmin)
        return 
    
    def findCoordinates(self):
        size = self.findNetworkSize()
        self.findOrigin()
        origin = self.dmin
        originNode = int(origin[0])
        self.nodes.append([originNode, 0.0, 0.0])
        
        networkTable = self.buildNetworkTable()
        if (networkTable[originNode-1][0] != 0):
            self.nodes.append([1,networkTable[originNode-1][0],0.0])
        else:
            self.nodes.append([2,networkTable[originNode-1][1],0.0])
        
        if ((networkTable[originNode-1][0] != 0) and (networkTable[originNode-1][1] != 0)):
            nodeq = 1
        else:
            nodeq = 2
        
        
        nodei = self.nodes[0][0] - 1
        nodep = self.nodes[1][0] - 1
        #print("i = ",nodei)
        #print("p = ",nodep)
        
        
        diq = networkTable[nodei][nodeq]
        dip = networkTable[nodei][nodep]
        dpq = networkTable[nodep][nodeq]
        #print("diq =",diq,"dip =",dip,"dpq =",dpq)
        theta = (networkTable[nodei][nodeq]**2 + networkTable[nodei][nodep]**2 - networkTable[nodep][nodeq]**2)/(2 * networkTable[nodei][nodeq] * networkTable[nodei][nodep])
        if (theta > 1):
            theta = 1
        if(theta < -1):
            theta = -1
        theta = math.acos(theta)
        qx = networkTable[nodei][nodeq] * math.cos(theta)
        qy = networkTable[nodei][nodeq] * math.sin(theta)
                
        self.nodes.append([(nodeq + 1),qx,qy])
        
        
        for j in range(0,size):
            if ( (j != nodei) and (j != nodep) and (j != nodeq)):
                dij = networkTable[nodei][j]
                dpj = networkTable[nodep][j]
                dqj = networkTable[nodeq][j]
                
                #print("diq =",diq,"dip =",dip,"dpq =",dpq)
                alpha = (dij**2 + dip**2 - dpj**2)/(2 * dij * dip)
                if (alpha > 1):
                    alpha = 1
                if(alpha < -1):
                    alpha = -1
                alpha = math.acos(alpha)
                
                beta = (diq**2 + dij**2 - dqj**2)/(2 * diq * dij)
                if (beta > 1):
                    beta = 1
                if(beta < -1):
                    beta = -1
                beta = math.acos(beta)
                
                jx = dij * math.cos(alpha)
                if (abs(beta - abs(alpha - theta)) > .05):
                    jy = -1 * dij * math.sin(alpha)
                else:
                    jy = dij * math.sin(alpha)
                
                self.nodes.append([(j + 1),jx,jy])
        return
            
        
    
    
    def findNetworkSize(self):
        networkSize = 0
        for i in range(0,len(self.links)):
            tempLink = self.links[i]
            if (int(tempLink[0]) > networkSize):
                networkSize = int(tempLink[0])
            if (int(tempLink[1]) > networkSize):
                networkSize = int(tempLink[1])
        #print(networkSize)
        return networkSize
    
    def buildNetworkTable(self):
        networkSize = self.findNetworkSize()
        NetworkTable = [[0 for col in range(networkSize)] for row in range(networkSize)]
        for i in range(0,len(self.links)):
            tempLink = self.links[i]
            NetworkTable[int(tempLink[0])-1][int(tempLink[1])-1] = float(tempLink[2])
            NetworkTable[int(tempLink[1])-1][int(tempLink[0])-1] = float(tempLink[2])
        #for i in range(0,len(NetworkTable)):
            #print(NetworkTable[i],"\n")
        return NetworkTable
    
    def writeCoordinatesFile(self,fileout):
        f = open(fileout, 'w')
        for i in range(0,len(self.nodes)):
            f.write(str(self.nodes[i][0]) + "," + str(self.nodes[i][1]) + "," + str(self.nodes[i][2]))
            f.write('\r\n')
        f.close()
        return
    




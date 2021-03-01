#!/usr/bin/env python

import requests #for sending requests (Flask request is for receive)
import json

def Arrayify(strArr):
    return map(int, strArr.strip("[]").split(","))

def GetHeader():
    return {'content-type': 'application/json'}

class MarsyasClient():
    def __init__(self, server):
        self.server = server
        self.headers = GetHeader()
        
    def initNet(self, spec):
        api = "InitNet"
        #Setup data packet
        data = {}
        data['netDef'] = spec
        data = json.dumps(data)
       
        requests.post('http://{SERVERLOCATION}/{api}'.format(SERVERLOCATION=self.server, api=api), headers=self.headers, data=data)
        return

    def linkctrl(self, dest, mrs_type):
        api = "LinkCtrl"
        #Setup data packet
        data = {}
        data['dest'] = dest
        data['mrs_type'] = mrs_type
        data = json.dumps(data)
       
        requests.post('http://{SERVERLOCATION}/{api}'.format(SERVERLOCATION=self.server, api=api), headers=self.headers, data=data)
        return

    def updatectrl(self, dest_key, dest_value):
        api = "UpdateCtrl"
        #Setup data packet
        data = {}
        data['dest_key'] = dest_key
        data['dest_value'] = dest_value
        data = json.dumps(data)
       
        requests.post('http://{SERVERLOCATION}/{api}'.format(SERVERLOCATION=self.server, api=api), headers=self.headers, data=data)
        return

    def funcgetctrl(self, Var, RunMethod, MethodVar="", MethodVarType="", IsReturn="False", IsTick="False"):
        api = "FuncGetCtrl"
        #Setup data packet
        data = {}
        data['Var'] = Var
        data['RunMethod'] = RunMethod
        data['MethodVar'] = MethodVar
        data['MethodVarType'] = MethodVarType
        data['IsReturn'] = IsReturn
        data['IsTick'] = IsTick
        data = json.dumps(data) 

        if(bool(IsReturn)):
            return requests.post('http://{SERVERLOCATION}/{api}'.format(SERVERLOCATION=self.server, api=api), headers=self.headers, data=data).text
        else:
            requests.post('http://{SERVERLOCATION}/{api}'.format(SERVERLOCATION=self.server, api=api), headers=self.headers, data=data)
            return


"""
    def getOutput(self, dest_key):
        api = "GetCtrl"
        #Setup data packet
        data = {}
        data['dest_key'] = dest_key
        data = json.dumps(data)

        output = requests.post('http://{SERVERLOCATION}/{api}'.format(SERVERLOCATION=self.server, api=api), headers=self.headers, data=data)
        return output

    def getNET(self):
        api = "GetNET"

        output = requests.post('http://{SERVERLOCATION}/{api}'.format(SERVERLOCATION=self.server, api=api), headers=self.headers)
        return output
"""

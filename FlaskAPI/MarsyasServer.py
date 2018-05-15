#!/usr/bin/env python

from marsyas import *
from marsyas_util import *
import os
from flask import Flask, render_template, request, redirect, url_for, session
import yaml
import importlib

def toBool(s):
    return s == "True"

app = Flask(__name__)
app.debug = True
app.secret_key = os.environ['KEY']

global_net = "1" #sessions do not support the storage of objects, thus resort to use global variables in a server instance
@app.route("/InitNet", methods=['POST'])
def InitNet():
    try:
        netSpec = yaml.safe_load(request.data)['netDef'] #Using yaml.safe_load to unpack json since it will maintain the type of the input from client rather than re-encoding itself through json.loads()
        global global_net
        global_net = create(netSpec)
        return "Network Created"
    except(error):
        print error
        raise 400

@app.route("/LinkCtrl", methods=['POST'])
def LinkCtrl():
    try:
        data = yaml.safe_load(request.data)
        dest = str(data['dest'])
        mrs_type = str(data['mrs_type'])
        global_net.linkControl(dest, mrs_type)
        return "Link Control added"
    except(error):
        print error
        raise 400

@app.route("/UpdateCtrl", methods=['POST'])
def UpdateCtrl():
    try:
        data = yaml.safe_load(request.data)
        dest_key = str(data['dest_key'])
        dest_value = str(data['dest_value'])
        global_net.updControl(dest_key, dest_value)
        return "Link Control added" 
    except(error):
        print error
        raise 400

#Functions involving GetCtrl
@app.route("/FuncGetCtrl", methods=['POST'])
def FuncGetCtrl():
    try:
        data = yaml.safe_load(request.data)
        Var = str(data['Var'])
        RunMethod = str(data['RunMethod'])
        MethodVar = str(data['MethodVar'])
        MethodVarType = str(data['MethodVarType'])
        IsReturn = toBool(data['IsReturn'])
        IsTick = toBool(data['IsTick'])
        
        #print data

        #exec_method = getattr(global_net, Func) #Setting up net.getControl()
        #temp = exec_method(Var) #Executing net.getControl()
        temp = global_net.getControl(Var)
        result = getattr(temp, RunMethod) #Setting up net.getControl().Function()
        print data
        print IsTick
        if(IsTick):
            print "TICK"
            global_net.tick()
       
        if(MethodVar != ""): #IE:/ not empty ""
           module = importlib.import_module('__builtin__') #For future expansion of other types, need to change '__builtin__' to be a variable for dynamic cases
           print "TYPE", MethodVarType
           convert = getattr(module, MethodVarType)
           MethodVar = convert(MethodVar) #Convert To the correct type
           print MethodVar

           result = result(MethodVar) #Run net.getControl.Function(MethodVar) || net.getControl.Function()
        else:
           result = result()
        
        if(IsReturn):
           return str(result)
        else:
           return "get control"

    except:
        print error
        raise 400


@app.errorhandler(400)
def handle_error_networknotcreated(error):
    print "NNC"
    return {'message': error.message}, 400

if __name__ == "__main__":
    app.run(host='0.0.0.0')

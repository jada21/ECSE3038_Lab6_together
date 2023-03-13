#Jada Campbell 620141014
import requests
from datetime import datetime
from fastapi import FastAPI, Body, Request, HTTPException, status
from fastapi.responses import Response, JSONResponse
import pydantic
from fastapi.encoders import jsonable_encoder
from fastapi.middleware.cors import CORSMiddleware
from bson import ObjectId
import motor.motor_asyncio

app = FastAPI()
origins = [
    "http://localhost:8000",
    "http://127.0.0.1:8000",
    "http://192.168.100.195:8000",
    "https://pain-api.onrender.com"
]

app.add_middleware(                         #instance of middle ware class  
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],                   # HTTP request types like get, post etc. * means all of dem
    allow_headers=["*"],
)

client = motor.motor_asyncio.AsyncIOMotorClient("mongodb+srv://jadasdata:Nilaja2002@cluster0.hj6aecx.mongodb.net/?retryWrites=true&w=majority")
db = client.Lab6
pydantic.json.ENCODERS_BY_TYPE[ObjectId]=str  

sunset_api_endpoint = "https://ecse-sunset-api.onrender.com/api/sunset"
snatch_sunset = requests.get(sunset_api_endpoint)
sunset_json = snatch_sunset.json()
sunset_time = datetime.strptime(sunset_json["sunset"], "%Y-%m-%dT%H:%M:%S.%f")


@app.get("/")
async def jadaissocool():
    return {"hellooo this is jada's lab 6, go to /api/put or /api/state route to see more \u2764\uFE0F"}

#get handler
@app.get("/api/state")
async def get_data():
    status_object = await db["Data"].find_one()
    #status_object["sunset"] = sunset_time              this was used for debugging
    if sunset_time < datetime.now():
        status_object["light"] = True
    else:
        status_object["light"] = False

    if status_object["temperature"] >= 28:
        status_object["fan"] = True
    else:
        status_object["fan"] = False
    return status_object


#put handler - will replace data in database with every 'PUT'
@app.put("/api/temperature", status_code=204)
async def put_data(request: Request):
    data_object = await request.json()
   
    new_data = await db["Data"].find_one({"_id": ObjectId("640e1fc87a0bf6493917690b")})
    if new_data:
        await db["Data"].update_one({"_id":ObjectId("640e1fc87a0bf6493917690b")}, {"$set": data_object})
    else:
        await db["Data"].insert_one({**data_object, "_id": ObjectId("640e1fc87a0bf6493917690b")})

    created_data = await db["Data"].find_one({"_id": ObjectId("640e1fc87a0bf6493917690b")})
    if not created_data:
        raise HTTPException(status_code=404, detail= "Not Found")
    return created_data
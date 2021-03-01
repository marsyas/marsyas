#Flask Integration#

The development of this Flask API is to facilitate the possibility of other languages interfacing to Marsyas. As of current native development, Marsyas is only available for C, C++, and Python. Redevelopment to accomodate languages like Javascript will be challenging as such language is not designed for the intent of handling large sequential datasets. Thus using the python flask as the backend of the API, languages such as Javascript will be able to access such functionality without compromising performance. 

MarsyasFlask is the python implementation of the client-side interface for requesting API calls to the MarsyasServer.

MarsyasServer is the python implementation of the API server whereby the client-side interface is language agnostic.

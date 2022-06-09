from flask import Flask
from pytinhas.ext.site import main



def create_app():
    app = Flask(__name__)
    main.init_app(app)
    return app
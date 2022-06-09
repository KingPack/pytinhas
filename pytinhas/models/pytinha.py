from pytinhas.ext.database import Base

from sqlalchemy import Column
from sqlalchemy import Integer

from sqlalchemy.sql.sqltypes import String

from marshmallow import Schema
from marshmallow import fields


class PytinhaModel(Base):

    __tablename__ = 'pytinha'

    id = Column(Integer, primary_key=True)
    name = Column(String)
    birthday = Column(String)
    description = Column(String)
    gender = Column(String)
    specie = Column(String)
    fase = Column(String)
    humidity = Column(String)
    temperature = Column(String)
    
class PytinhaSchema(Schema):

    id = fields.Integer()
    name = fields.String()
    birthday = fields.String()
    description = fields.String()
    gender = fields.String()
    specie = fields.String()
    fase = fields.String()
    humidity = fields.String()
    temperature = fields.String()

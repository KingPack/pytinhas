#!/usr/bin/env python3

from flask import (Flask, flash, jsonify, redirect, render_template,
                   request, url_for)
from wtforms import StringField, validators
from flask_wtf import FlaskForm
from .ext.database import SessionLocal, Base, engine
from pytinhas.models import pytinha


class Create_pytinha(FlaskForm):
    name = StringField('Nome', [validators.Length(min=4, max=25)])
    birthday = StringField('Nascimento', [validators.Length(min=4, max=25)])
    description = StringField('Descrição', [validators.Length(min=1, max=25)])
    gender = StringField('Genero', [validators.Length(min=4, max=25)])
    fase = StringField('Fase', [validators.Length(min=4, max=25)])
    specie = StringField('Especie', [validators.Length(min=1, max=25)])
    humidity = StringField('Humidade', [validators.Length(min=1, max=25)])
    temperature = StringField('Temperatura', [validators.Length(min=1,
                                                                max=25)])


# ----------------------------------------------------------------------------#
# Initialize app and set config
def create_app():
    schema_pytinha = pytinha.PytinhaSchema(many=True)
    db = SessionLocal()
    Base.metadata.create_all(bind=engine)
    app = Flask(__name__)
    app.config['SECRET_KEY'] = 'test'
    app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

# ----------------------------------------------------------------------------#
# Routes Main
    @app.route('/', methods=['GET', 'POST'])
    def index():

        return render_template('index.html')

    @app.route('/create_confirm', methods=['GET', 'POST'])
    def create_confirm():

        return render_template('pytinha/create_confirm.html')

    @app.route('/create', methods=['GET', 'POST'])
    def create_pytinha():

        form = Create_pytinha(request.form)

        if request.method == 'POST' and form.validate():
            create_pytinha = pytinha.PytinhaModel(
                name=form.name.data,
                birthday=form.birthday.data,
                description=form.description.data,
                gender=form.gender.data,
                specie=form.specie.data,
                fase=form.fase.data,
                humidity=form.humidity.data,
                temperature=form.temperature.data
            )
            db.add(create_pytinha)
            db.commit()
            db.close()
            flash('Thanks for registering')

            return redirect(url_for('create_confirm'))

        else:
            return render_template('pytinha/create.html', form=form)

    @app.route('/list', methods=['GET', 'POST'])
    def list_pytinha():
        pytinhas = db.query(pytinha.PytinhaModel).all()
        pytinhas_json = schema_pytinha.dump(pytinhas)

        return jsonify(pytinhas_json)

    return app


# ----------------------------------------------------------------------------#

if __name__ == '__main__':
    create_app.run()

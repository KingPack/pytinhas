from flask import Blueprint

bp = Blueprint('main', __name__)


def init_app(app):
    app.register_blueprint(bp)


@bp.route('/')
def index():
    return 'Interface pytinhas'
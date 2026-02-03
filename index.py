from flask import Flask, request, render_template

app = Flask(__name__)

soil = 0
ldr = 0

@app.route('/')
def index():
    return render_template('index.html', soil=soil, ldr=ldr)

@app.route('/data')
def data():
    global soil, ldr

    soil = request.args.get('soil', default=0, type=int)
    ldr = request.args.get('ldr', default=0, type=int)

    print(f"Soil: {soil} | LDR: {ldr}")
    return
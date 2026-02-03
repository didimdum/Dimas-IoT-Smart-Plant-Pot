from flask import Flask, request, render_template

app = Flask(__name__)

soil_value = 0
ldr_value = 0

@app.route('/')
def index():
    return render_template('index.html', soil=soil_value, ldr=ldr_value)

@app.route('/data')
def data():
    global soil_value, ldr_value

    soil_value = request.args.get('soil', default=0, type=int)
    ldr_value = request.args.get('ldr', default=0, type=int)

    print(f"Soil: {soil_value} | LDR: {ldr_value}")
    return
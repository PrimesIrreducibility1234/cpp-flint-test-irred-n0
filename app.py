from flask import Flask, request, Response
import subprocess
import tempfile
import os

app = Flask(__name__)

HTML = """
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>C++ FLINT Support Generator</title>
  <style>
    body { font-family: system-ui, sans-serif; max-width: 900px; margin: 40px auto; padding: 0 20px; }
    input, button { font-size: 16px; padding: 10px; margin: 5px; }
    form { margin-bottom: 35px; padding: 20px; border: 1px solid #ddd; border-radius: 12px; }
  </style>
</head>
<body>
  <h1>Data Generator for Minkowski Indecomposability of Polynomials</h1>

  <form method="POST" action="/generate">
    <h2>Support generator</h2>
    <p>Enter DEG, then the site downloads out.txt for the support sets that are irreducible not accounted for by the Crowdmath and previous PRIMES papers.</p>

    <label>DEG:</label>
    <input name="deg" type="number" value="20" min="1" max="25">

    <button type="submit">Generate out.txt</button>
  </form>

  <form method="POST" action="/generate_coeffs">
    <h2>Coefficient polynomial generator</h2>
    <p>Enter maxdeg and maxcoeff. This runs temp.cpp and downloads coeff.txt.</p>

    <label>maxdeg:</label>
    <input name="maxdeg" type="number" value="8" min="1" max="12">

    <label>maxcoeff:</label>
    <input name="maxcoeff" type="number" value="5" min="1" max="20">

    <button type="submit">Generate coeff.txt</button>
  </form>
</body>
</html>
"""

@app.get("/")
def home():
    return HTML


@app.post("/generate")
def generate():
    deg = request.form.get("deg", "20").strip()

    try:
        deg_int = int(deg)
    except ValueError:
        return Response("Invalid DEG", mimetype="text/plain", status=400)

    if deg_int < 1 or deg_int > 25:
        return Response("Use 1 <= DEG <= 25 for now.", mimetype="text/plain", status=400)

    with tempfile.TemporaryDirectory() as tmp:
        result = subprocess.run(
            ["/app/main"],
            input=f"{deg_int}\n",
            text=True,
            cwd=tmp,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=120
        )

        out_path = os.path.join(tmp, "out.txt")

        if result.returncode != 0:
            return Response(
                "Program failed.\n\nSTDOUT:\n" + result.stdout + "\n\nSTDERR:\n" + result.stderr,
                mimetype="text/plain",
                status=500
            )

        if not os.path.exists(out_path):
            return Response(
                "Program ran but did not create out.txt.\n\nSTDOUT:\n"
                + result.stdout
                + "\n\nSTDERR:\n"
                + result.stderr,
                mimetype="text/plain",
                status=500
            )

        with open(out_path, "r") as f:
            text = f.read()

    return Response(
        text,
        mimetype="text/plain",
        headers={"Content-Disposition": "attachment; filename=out.txt"}
    )


@app.post("/generate_coeffs")
def generate_coeffs():
    maxdeg = request.form.get("maxdeg", "8").strip()
    maxcoeff = request.form.get("maxcoeff", "5").strip()

    try:
        maxdeg_int = int(maxdeg)
        maxcoeff_int = int(maxcoeff)
    except ValueError:
        return Response("Invalid maxdeg or maxcoeff", mimetype="text/plain", status=400)

    if maxdeg_int < 1 or maxdeg_int > 12:
        return Response("Use 1 <= maxdeg <= 12 for now.", mimetype="text/plain", status=400)

    if maxcoeff_int < 1 or maxcoeff_int > 20:
        return Response("Use 1 <= maxcoeff <= 20 for now.", mimetype="text/plain", status=400)

    with tempfile.TemporaryDirectory() as tmp:
        result = subprocess.run(
            ["/app/temp"],
            input=f"{maxdeg_int} {maxcoeff_int}\n",
            text=True,
            cwd=tmp,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=120
        )

        coeff_path = os.path.join(tmp, "coeff.txt")

        if result.returncode != 0:
            return Response(
                "Program failed.\n\nSTDOUT:\n" + result.stdout + "\n\nSTDERR:\n" + result.stderr,
                mimetype="text/plain",
                status=500
            )

        if not os.path.exists(coeff_path):
            return Response(
                "Program ran but did not create coeff.txt.\n\nSTDOUT:\n"
                + result.stdout
                + "\n\nSTDERR:\n"
                + result.stderr,
                mimetype="text/plain",
                status=500
            )

        with open(coeff_path, "r") as f:
            text = f.read()

    return Response(
        text,
        mimetype="text/plain",
        headers={"Content-Disposition": "attachment; filename=coeff.txt"}
    )

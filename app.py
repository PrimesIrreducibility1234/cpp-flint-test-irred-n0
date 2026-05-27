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
    code { background: #f5f5f5; padding: 2px 5px; border-radius: 5px; }
  </style>
</head>
<body>
  <h1>Data Generator for Minkowski Indecomposability of Polynomials</h1>

  <form method="POST" action="/generate">
    <h2>Support generator</h2>
    <p>
      Enter DEG, then the site downloads out.txt for the support sets that are irreducible not accounted for by the CrowdMath and previous PRIMES papers.
    </p>

    <label>DEG:</label>
    <input name="deg" type="number" value="20" min="1" max="25">

    <button type="submit">Generate out.txt</button>
  </form>

  <form method="POST" action="/generate_coeffs">
    <h2>Coefficient polynomial generator</h2>

    <p>
      Enter maxdeg and maxcoeff. This runs temp.cpp and downloads coeff.txt.
    </p>

    <p>
      Each output line has the polynomial, followed by a 9-digit boolean code.
      The digits are concatenated 0/1 results saying whether each irreducibility test detected the polynomial.
    </p>

    <p>
      The order of the boolean code is:
      <br>
      <code>PrimeValue, Eisenstein, Perron, Ostrowski, Bevelacqua, Monolithic, CrowdMath, Kolekar, Bonciocat</code>
    </p>

    <p>
      Example: <code>100000001</code> means the Prime Value Test and Bonciocat's Criterion detected irreducibility, but the other seven tests did not.
    </p>

    <label>maxdeg:</label>
    <input name="maxdeg" type="number" value="3" min="1" max="6">

    <label>maxcoeff:</label>
    <input name="maxcoeff" type="number" value="2" min="1" max="6">

    <button type="submit">Generate coeff.txt</button>
  </form>

  <form method="POST" action="/generate_minkowski">
    <h2>Minkowski decomposition checker</h2>

    <p>
      Enter a support set as space-separated exponents. Example:
      <code>20 10 0</code>.
    </p>

    <p>
      This runs minkowski.cpp and returns whether the support has a nontrivial Minkowski decomposition.
      If it does, it prints sets <code>A</code> and <code>B</code> such that <code>support = A + B</code>.
    </p>

    <label>Support:</label>
    <input name="support" type="text" value="20 10 0" style="width: 300px;">

    <button type="submit">Check Minkowski decomposition</button>
  </form>
  <form method="POST" action="/check_irred">
  <h2>Single polynomial irreducibility checker</h2>

  <p>
    Enter coefficients in increasing degree order. Example:
    <code>1 3 3 1</code> means
    <code>1 + 3x + 3x^2 + x^3</code>.
  </p>

  <p>
    This runs irred.cpp and prints which irreducibility criteria detect the polynomial,
    then checks reducibility in <code>Z[x]</code> and <code>N0[x]</code>.
  </p>

  <label>Coefficients:</label>
  <input name="coeffs" type="text" value="1 3 3 1" style="width: 350px;">

  <button type="submit">Check polynomial</button>
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
        try:
            result = subprocess.run(
                ["/app/main"],
                input=f"{deg_int}\n",
                text=True,
                cwd=tmp,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=120
            )
        except subprocess.TimeoutExpired as e:
            return Response(
                "Program timed out.\n\n"
                + "STDOUT:\n" + str(e.stdout or "")
                + "\n\nSTDERR:\n" + str(e.stderr or ""),
                mimetype="text/plain",
                status=500
            )

        out_path = os.path.join(tmp, "out.txt")

        if result.returncode != 0:
            return Response(
                "Program failed.\n\n"
                + "Return code: " + str(result.returncode)
                + "\n\nSTDOUT:\n" + result.stdout
                + "\n\nSTDERR:\n" + result.stderr,
                mimetype="text/plain",
                status=500
            )

        if not os.path.exists(out_path):
            return Response(
                "Program ran but did not create out.txt.\n\n"
                + "Return code: " + str(result.returncode)
                + "\n\nSTDOUT:\n" + result.stdout
                + "\n\nSTDERR:\n" + result.stderr,
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
    maxdeg = request.form.get("maxdeg", "3").strip()
    maxcoeff = request.form.get("maxcoeff", "2").strip()

    try:
        maxdeg_int = int(maxdeg)
        maxcoeff_int = int(maxcoeff)
    except ValueError:
        return Response("Invalid maxdeg or maxcoeff", mimetype="text/plain", status=400)

    if maxdeg_int < 1 or maxdeg_int > 6:
        return Response("Use 1 <= maxdeg <= 6 for now.", mimetype="text/plain", status=400)

    if maxcoeff_int < 1 or maxcoeff_int > 6:
        return Response("Use 1 <= maxcoeff <= 6 for now.", mimetype="text/plain", status=400)

    with tempfile.TemporaryDirectory() as tmp:
        try:
            result = subprocess.run(
                ["/app/temp"],
                input=f"{maxdeg_int} {maxcoeff_int}\n",
                text=True,
                cwd=tmp,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=120
            )
        except subprocess.TimeoutExpired as e:
            return Response(
                "Program timed out.\n\n"
                + "Try smaller maxdeg/maxcoeff.\n\n"
                + "STDOUT:\n" + str(e.stdout or "")
                + "\n\nSTDERR:\n" + str(e.stderr or ""),
                mimetype="text/plain",
                status=500
            )

        coeff_path = os.path.join(tmp, "coeff.txt")

        if result.returncode != 0:
            return Response(
                "Program failed.\n\n"
                + "Return code: " + str(result.returncode)
                + "\n\nSTDOUT:\n" + result.stdout
                + "\n\nSTDERR:\n" + result.stderr,
                mimetype="text/plain",
                status=500
            )

        if not os.path.exists(coeff_path):
            return Response(
                "Program ran but did not create coeff.txt.\n\n"
                + "Return code: " + str(result.returncode)
                + "\n\nSTDOUT:\n" + result.stdout
                + "\n\nSTDERR:\n" + result.stderr,
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


@app.post("/generate_minkowski")
def generate_minkowski():
    support_text = request.form.get("support", "").strip()

    if not support_text:
        return Response("Enter a support set, like: 20 10 0", mimetype="text/plain", status=400)

    try:
        nums = [int(x) for x in support_text.split()]
    except ValueError:
        return Response("Support must be space-separated integers.", mimetype="text/plain", status=400)

    if any(x < 0 for x in nums):
        return Response("Support exponents must be nonnegative.", mimetype="text/plain", status=400)

    if len(nums) < 2:
        return Response("Support needs at least two exponents.", mimetype="text/plain", status=400)

    if max(nums) > 63:
        return Response("Use maximum exponent <= 63 for this bitmask version.", mimetype="text/plain", status=400)

    with tempfile.TemporaryDirectory() as tmp:
        try:
            result = subprocess.run(
                ["/app/minkowski"],
                input=" ".join(map(str, nums)) + "\n",
                text=True,
                cwd=tmp,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=120
            )
        except subprocess.TimeoutExpired as e:
            return Response(
                "Program timed out.\n\n"
                + "Try a smaller support set.\n\n"
                + "STDOUT:\n" + str(e.stdout or "")
                + "\n\nSTDERR:\n" + str(e.stderr or ""),
                mimetype="text/plain",
                status=500
            )

        if result.returncode != 0:
            return Response(
                "Program failed.\n\n"
                + "Return code: " + str(result.returncode)
                + "\n\nSTDOUT:\n" + result.stdout
                + "\n\nSTDERR:\n" + result.stderr,
                mimetype="text/plain",
                status=500
            )

    return Response(
        result.stdout,
        mimetype="text/plain",
        headers={"Content-Disposition": "attachment; filename=minkowski.txt"}
    )
  @app.post("/check_irred")
def check_irred():
    coeff_text = request.form.get("coeffs", "").strip()

    if not coeff_text:
        return Response("Enter coefficients, like: 1 3 3 1", mimetype="text/plain", status=400)

    try:
        coeffs = [int(x) for x in coeff_text.split()]
    except ValueError:
        return Response("Coefficients must be space-separated integers.", mimetype="text/plain", status=400)

    if any(c < 0 for c in coeffs):
        return Response("Coefficients must be nonnegative integers.", mimetype="text/plain", status=400)

    if len(coeffs) > 64:
        return Response("Use degree <= 63 for now.", mimetype="text/plain", status=400)

    if max(coeffs) > 1000000:
        return Response("Use coefficients <= 1000000 for now.", mimetype="text/plain", status=400)

    with tempfile.TemporaryDirectory() as tmp:
        try:
            result = subprocess.run(
                ["/app/irred"],
                input=" ".join(map(str, coeffs)) + "\n",
                text=True,
                cwd=tmp,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=120
            )
        except subprocess.TimeoutExpired as e:
            return Response(
                "Program timed out.\n\n"
                + "Try a smaller polynomial.\n\n"
                + "STDOUT:\n" + str(e.stdout or "")
                + "\n\nSTDERR:\n" + str(e.stderr or ""),
                mimetype="text/plain",
                status=500
            )

        if result.returncode != 0:
            return Response(
                "Program failed.\n\n"
                + "Return code: " + str(result.returncode)
                + "\n\nSTDOUT:\n" + result.stdout
                + "\n\nSTDERR:\n" + result.stderr,
                mimetype="text/plain",
                status=500
            )

    return Response(
        result.stdout,
        mimetype="text/plain",
        headers={"Content-Disposition": "attachment; filename=irred.txt"}
    )

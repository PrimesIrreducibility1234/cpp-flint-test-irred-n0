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
    pre { background: #f6f6f6; padding: 16px; border-radius: 10px; white-space: pre-wrap; max-height: 600px; overflow: auto; }
  </style>
</head>
<body>
  <h1>C++ FLINT Support Generator</h1>
  <form method="POST" action="/generate">
    <label>DEG:</label>
    <input name="deg" type="number" value="20" min="1" max="25">
    <button type="submit">Generate out.txt</button>
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
            return Response("Program ran but did not create out.txt.", mimetype="text/plain", status=500)

        with open(out_path, "r") as f:
            text = f.read()

    return Response(text, mimetype="text/plain")

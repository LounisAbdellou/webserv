<?php
header("HTTP 200 OK");

// Démarrer la session
session_start();

// Modifier les cookies
setcookie("fontWeight", "normal", time() + 3600, "/");
setcookie("backgroundColor", "white", time() + 3600, "/");
setcookie("color", "black", time() + 3600, "/");

// Mettre à jour la session
$_SESSION["message"] = "Les cookies ont été mis à jour!";

$html = "<!DOCTYPE html>
<html lang=\"fr\">
<head>
    <meta charset=\"UTF-8\">
    <title>Page 3</title>
    <style>
        body {
            font-weight: " . $_COOKIE["fontWeight"] . ";
            background-color: " . $_COOKIE["backgroundColor"] . ";
            color: " . $_COOKIE["color"] . ";
        }
    </style>
</head>
<body>
    <h1>message => " . $_SESSION["message"] . "</h1>
    <p>Les valeurs des cookies ont été mises à jour.</p>
    <a href=\"page4.php\">Aller à la page 4</a>
</body>
</html>";

header("Content-Length: " . strlen($html));

echo $html;

?>

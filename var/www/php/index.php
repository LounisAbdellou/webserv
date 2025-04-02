<?php
// Démarrer la session
session_start();

// Définir les cookies
setcookie("fontWeight", "bold", time() + 3600, "/");
setcookie("backgroundColor", "lightgray", time() + 3600, "/");
setcookie("color", "darkblue", time() + 3600, "/");

// Stocker des informations dans la session
$_SESSION['message'] = "Bienvenue sur notre site!";
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Page d'accueil</title>
    <style>
    </style>
</head>
<body>
    <p>Ceci est la page d'accueil.</p>
    <a href="page2.php">Aller à la page 2</a>
</body>
</html>

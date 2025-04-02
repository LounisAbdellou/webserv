<?php
// Démarrer la session
session_start();

// Modifier les cookies
setcookie("fontWeight", "normal", time() + 3600, "/");
setcookie("backgroundColor", "white", time() + 3600, "/");
setcookie("color", "black", time() + 3600, "/");

// Mettre à jour la session
$_SESSION['message'] = "Les cookies ont été mis à jour!";
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Page 3</title>
    <style>
        body {
            font-weight: <?php echo $_COOKIE['fontWeight']; ?>;
            background-color: <?php echo $_COOKIE['backgroundColor']; ?>;
            color: <?php echo $_COOKIE['color']; ?>;
        }
    </style>
</head>
<body>
    <h1><?php echo $_SESSION['message']; ?></h1>
    <p>Les valeurs des cookies ont été mises à jour.</p>
    <a href="index.php">Retour à la page d'accueil</a>
</body>
</html>


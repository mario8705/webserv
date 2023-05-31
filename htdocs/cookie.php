<?php
    session_start();

    if (!isset($_SESSION['d'])) {
        $_SESSION['d'] = 0;
    }

    $_SESSION['d'] = $_SESSION['d'] + 1;

    echo 'Vous etes le ' . $_SESSION['d'] . ' visiteur !';

    var_dump($_SESSION);
?>
o
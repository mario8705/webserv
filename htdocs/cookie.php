<?php
    session_name('phpsessid');
    session_start(array(
        'session.name' => 'phpsessid',
    ));

    if (!isset($_SESSION['d'])) {
        $_SESSION['d'] = 0;
    }

    $_SESSION['d'] = $_SESSION['d'] + 1;

    echo 'Vous avez visite le site  ' . $_SESSION['d'] . ' fois !';

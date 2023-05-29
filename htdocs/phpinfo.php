<?php
   session_start();
    var_dump($_SERVER);
    var_dump($_SESSION);

    if (!isset($_SESSION['count']))
    {
        $_SESSION['count'] = 0;
    }
    $_SESSION['count'] = $_SESSION['count'] + 1;
    phpinfo();
?>
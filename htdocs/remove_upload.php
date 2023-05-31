<?php
    if ($_SERVER['REQUEST_METHOD'] != 'DELETE') {
        die('Not delete :(');
    }

    echo 'This is delete !';
    unlink('./uploads/178c963ff8178034efdda26fbb96d3054af025c5.cpp');
?>
<?php
    foreach ($_FILES as $key => $f) {
        $hash = sha1_file($f['tmp_name']);
        if (file_exists($hash))
            continue ;
        $ext = pathinfo($f['name'], PATHINFO_EXTENSION);
        if (!move_uploaded_file($f['tmp_name'], './uploads/' . $hash . '.' . $ext))
            echo 'Cannot upload file';
    }
?>
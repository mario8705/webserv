<!DOCTYPE html>
<html>
<head>
    <title>Testing query string</title>
    <meta charset="utf-8">
</head>
<body>
    <h1>List of Query parameters</h1>
    <table border=1>
        <tr>
            <th>Name</th>
            <th>Value</th>
        </tr>
<?php
    if (isset($_GET)):
        foreach ($_GET as $key => $value):
?>
            <tr>
                <td><?= $key ?></td>
                <td><?= $value ?></td>
            </tr>
<?php
        endforeach;
    endif;
?>
    </table>
</body>
</html>

<!DOCTYPE html>
<html>
<head>
    <title>Testing query string</title>
    <meta charset="utf-8">
</head>
<body>
    <h1>List of Query parameters</h1>

<?php
    if (isset($_POST) && !empty($_POST)):
?>
        <table border=1>
            <tr>
                <th>Name</th>
                <th>Value</th>
            </tr>
<?php
        foreach ($_POST as $key => $value):
?>
            <tr>
                <td><?= $key ?></td>
                <td><?= $value ?></td>
            </tr>
<?php
        endforeach;
?>
    </table>
<?php
    else:
?>
        <form action="test-post.php" method="post">
            <input type="text" name="name" placeholder="Name">
            <input type="text" name="city" placeholder="City">
            <button type="submit">Submit</button>
        </form>
<?php
    endif;
?>
</body>
</html>

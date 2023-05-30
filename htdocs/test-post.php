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
                <td><?= var_dump($value) ?></td>
            </tr>
<?php
        endforeach;
?>
    </table>
<?php
    else:
?>
        <form action="test-post.php" method="post" enctype="multipart/form-data">
            <input type="text" name="name" placeholder="Name">
            <input type="text" name="city" placeholder="City">
            <input type="file" name="thumbnail">
            <button type="submit">Submit</button>
        </form>
<?php
    endif;
?>
</body>
</html>

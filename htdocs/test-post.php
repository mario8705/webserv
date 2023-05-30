<?php
    if (isset($_FILES) && isset($_FILES['thumbnail'])) {
        move_uploaded_file($_FILES['thumbnail']['tmp_name'], 'upload.png');
    }
?>

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
    <h2>Files</h2>
    <hr>
    <p>
        <?php var_dump($_FILES); ?>
    </p>
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
    <p>Latest upload : </p>
    <img style="max-width: 100%; height: auto;" src="upload.png" alt="">
</body>
</html>

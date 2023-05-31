<?php
if ($_SERVER['REQUEST_METHOD'] === 'DELETE') {
    // Supprimer les fichiers envoyés avec la requête DELETE
    foreach ($_FILES as $key => $file) {
        $file_path = './uploads/' . $file['name'];
        if (file_exists($file_path)) {
            unlink($file_path);
        }
    }

    // Envoyer une réponse appropriée
    header('Content-Type: application/json');
    echo json_encode(['message' => 'Delete command executed successfully']);
    exit();
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

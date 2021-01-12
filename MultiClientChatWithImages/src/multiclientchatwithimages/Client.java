/*
Class:      CLient
Developer: Chase Dickerson
*/
package multiclientchatwithimages;

import java.io.File;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.Optional;
import javafx.application.Application;
import static javafx.application.Application.launch;
import javafx.application.Platform;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.control.TextInputDialog;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Pane;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

/**
 *
 * @author Chase Dickerson
 */
public class Client extends Application {
    
    // Global variables
    private String username = "";
    
    private TextField name = new TextField();
    private TextField sendMessage = new TextField();
    private Button sendButton = new Button("Send Message");
    private Button fileButton = new Button("Choose file");
    private TextArea textArea = new TextArea();
    
    private Socket sock;
    private ObjectOutputStream outputToFile;
    private ObjectInputStream inputFromClient;
    
    @Override
    public void start(Stage primaryStage) throws Exception {
        
        // Initial start up dialog asking for name
        TextInputDialog dialog = new TextInputDialog("Username");
        dialog.setTitle("Name");
        dialog.setHeaderText("Name");
        dialog.setContentText("Please enter your name:");
        
        Optional<String> result = dialog.showAndWait();
        result.ifPresent(name -> {
            username = name;
            System.out.println("Your name: " + name);
        });
        
        textArea.setEditable(false);
        textArea.wrapTextProperty();
        textArea.setPrefHeight(360);
        textArea.setPrefWidth(530);
        sendMessage.setPrefSize(360, 20);
        
        GridPane gridPane = new GridPane();
        gridPane.add(sendMessage, 0, 0);
        gridPane.add(sendButton, 1, 0);
        gridPane.add(fileButton, 2, 0);
        
        Pane pane = new HBox(10);
        pane.setPadding(new Insets(5, 5, 5, 5));
        
        BorderPane borderPane = new BorderPane();
        borderPane.setCenter(new ScrollPane(textArea));
        borderPane.setRight(pane);
        borderPane.setBottom(gridPane);
        
        // Create a scene and place it in the stage
        Scene scene = new Scene(borderPane, 630, 420);
        primaryStage.setTitle("Client: " + username); 
        primaryStage.setScene(scene);
        primaryStage.show(); 
        
        // Event listeners fired on specific action
        sendMessage.setOnAction(e -> chatFunctionality());
        
        sendButton.setOnAction(e -> chatFunctionality());
        
        fileButton.setOnAction(e -> getFile(primaryStage));
        
        try {
            // Creating new socket to server local host and on port 8000
            sock = new Socket("localhost", 8000);

            outputToFile = new ObjectOutputStream(sock.getOutputStream());
            inputFromClient = new ObjectInputStream(sock.getInputStream());
            
            // New thread to continously serve client
            new Thread( () -> {
                try{
                    while(true){
                        // Read ObjectTransfer object from socket
                        ObjectTransfer object = (ObjectTransfer) inputFromClient.readObject();
                        String message = object.getMessage();
                        File file = object.getFile();
                        if(message != null){
                            // Displaying message from other clients
                            textArea.appendText(message);
                        }
                        if(file != null){
                            // Showing image on GUI
                            Platform.runLater(() -> {
                                pane.getChildren().clear();
                                textArea.appendText("File: " + file.getPath() + '\n');
                                String filepath = "file:////";
                                filepath += (file.getPath().replace('/', '\\'));
                                Image image = new Image(filepath);
                                ImageView img = new ImageView(image);
                                img.setFitHeight(100);
                                img.setFitWidth(100);
                                pane.getChildren().add(img);
                            });
                        }
                    }
                }catch(IOException e){
                    System.err.println(e);
                } catch (ClassNotFoundException ex) {
                    System.err.println(ex);
                }
            }).start();
            
        } catch(IOException e){
            System.err.println(e);
        }  
    }
    
    // Used to send messages to server
    public void chatFunctionality() {
        try{
            String text = username + ": " + sendMessage.getText() + '\n';
            ObjectTransfer ot = new ObjectTransfer(text);
            outputToFile.writeObject(ot);
            sendMessage.clear();
        }catch(IOException e){
            e.printStackTrace();
        }
    }
    
    // Used to send images to server
    public void getFile(Stage primaryStage) {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Open Resource File");
        File file = fileChooser.showOpenDialog(primaryStage);
        
        try{
            ObjectTransfer ot = new ObjectTransfer(file);
            outputToFile.writeObject(ot);
        }catch(Exception e){
            System.err.println(e);
        }
    }
    
    public static void main(String[] args) {
        launch(args);
    }
}

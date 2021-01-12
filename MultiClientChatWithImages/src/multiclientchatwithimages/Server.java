/*
Class:      Server
Developer:  Chase Dickerson
Date:       12/5/2019
Purpose:    Executes all of the functionality to serve clients
 */
package multiclientchatwithimages;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Application;
import static javafx.application.Application.launch;
import javafx.application.Platform;
import javafx.scene.Scene;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TextArea;
import javafx.stage.Stage;

/**
 *
 * @author Chase Dickerson
 */
public class Server extends Application {
    
    // Global variables
    private TextArea textArea = new TextArea();
    private ScrollPane scrollPane = new ScrollPane();
    
    private ServerSocket server;
    
    // Mapping of sockets to output streams
    private Map<Socket, ObjectOutputStream> outputClients = new HashMap<>();
    
    @Override
    public void start(Stage primaryStage) throws Exception {
        
        // Create a scene and place it in the stage
        Scene scene = new Scene(new ScrollPane(textArea), 400, 400);
        primaryStage.setTitle("Server"); 
        primaryStage.setScene(scene);
        primaryStage.show(); 
        
        // Creating a new thread to serve clients
        new Thread( () -> {
            try{
                // Making a server on port 8000
                server = new ServerSocket(8000);
                textArea.appendText("Server started at " + new Date() + '\n');
                while(true){
                    Socket sock = server.accept();
                    ObjectOutputStream outputToFile = new ObjectOutputStream(sock.getOutputStream());
                    outputClients.put(sock, outputToFile);
                    
                    // Used to display text to server text area
                    Platform.runLater(() -> {
                        textArea.appendText("Serving new client\n");
                        InetAddress inetAddress = sock.getInetAddress();
                        textArea.appendText("Client name is " + inetAddress.getHostName() + '\n');
                        textArea.appendText("Client IP address is " + inetAddress.getHostAddress()+ '\n');
                    });
                    new Thread(new ServeClient(sock, this)).start();
                }
            } catch(IOException e){ 
                e.printStackTrace();
            }
        }).start();
    }
    
    /*
    Class:      ServeClient
    Purpose:    Used to serve each individual client
    */
    class ServeClient implements Runnable {
    
        private Socket sock;
        private Server server;

        public ServeClient(Socket sock, Server server){
            this.sock = sock;
            this.server = server;
        }

        @Override
        public void run() {
            try {
                // Getting what the client sent to the server
                ObjectInputStream inputFromClient = new ObjectInputStream(sock.getInputStream());

                while(true){
                    ObjectTransfer object = (ObjectTransfer) inputFromClient.readObject();
                    
                    // Looping over map to send ObjectTransfer object to clients
                    for(Map.Entry<Socket, ObjectOutputStream> client : outputClients.entrySet()){
                        ObjectOutputStream outputStream = (ObjectOutputStream) client.getValue();
                        try{
                            outputStream.writeObject(object);
                        } catch(IOException e){ 
                            System.err.println(e);
                        }
                    }

                    textArea.appendText("Message sent to all Clients!\n");
                }
            } catch(IOException e){ 
                System.err.println(e);
            } catch (ClassNotFoundException ex) {
                Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    public static void main(String[] args) 
    {
        launch(args);
    }
}

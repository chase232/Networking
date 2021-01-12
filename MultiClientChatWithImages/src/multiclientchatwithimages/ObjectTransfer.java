/*
Class:      ObjectTransfer
Developer:  Chase Dickerson
Date:       12/5/2019
Purpose:    Serves as an object that can be sent through sockets to carry a message
                or file
 */
package multiclientchatwithimages;

import java.io.File;

/**
 *
 * @author Chase Dickerson
 */
public class ObjectTransfer implements java.io.Serializable{
    
    // Class variables used for message and image
    private String message;
    private File file;

    public ObjectTransfer(String message, File file) {
        this.message = message;
        this.file = file;
    }

    public ObjectTransfer(String message) {
        this.message = message;
    }
    
    public ObjectTransfer(File file) {
        this.file = file;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }
    
    public File getFile() {
        return file;
    }

    public void setFile(File file) {
        this.file = file;
    }
}

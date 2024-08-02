package encryptionUtilities;

import java.awt.Dimension;
import java.awt.Font;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.lang.IllegalArgumentException;
import java.lang.NullPointerException;
import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.Base64;
import java.util.Scanner;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileNameExtensionFilter;

/**
 * Class of utilities used for reading and writing the keys
 * checking if the key file exists, and deleting the key file
 * @author DarkAssassin23 - Will Jones
 *
 */
public class Utilities implements Serializable
{
    // Added to fix Serializable warning
    private static final long serialVersionUID = 1L;

    final private EEA eea = new EEA();
    final private int ROUNDS = 5; // Rounds for key encryption and decryption
    final private Base64.Encoder base64Encode = Base64.getEncoder();
    final private Base64.Decoder base64Decode = Base64.getDecoder();
    // Fixes bug where double-clicking on the .jar file would
    // set the working directory as a temproary location and
    // not where the file is
    static final String
        legacyKeyFilePath = new File(Utilities.class.getProtectionDomain()
                                         .getCodeSource()
                                         .getLocation()
                                         .getPath())
                                .toString()
                                .replace("EEA_App.jar", "keys.ser");

    /**
     * Return the SHA512 Hash of the given password
     * @param password The password to hash
     * @return The SHA512 hash of the password
     */
    private String hashPassword(String password)
    {
        try
        {
            byte[] hashData = MessageDigest.getInstance("SHA-512").digest(
                password.getBytes());
            BigInteger pass = new BigInteger(1, hashData);
            StringBuilder temp = new StringBuilder(pass.toString(16));
            while (temp.length() % 64 != 0)
                temp.insert(0, '0');
            return new String(temp);
        }
        catch (NoSuchAlgorithmException e1)
        {
            return null;
        }
    }

    /**
     * Get the password used to encrypt or decrypt the keys file
     * @param encrypting Is this to encrypt the password
     * @return The SHA512 hash of the password
     */
    private String getKeysPassword(boolean encrypting)
    {
        JPanel panel = new JPanel();
        JLabel label = new JLabel("Password:");
        JPasswordField pass = new JPasswordField(
            10); // Make sure the field has a width
        panel.add(label);
        panel.add(pass);
        String[] options = new String[] { "OK", "Cancel" };
        int option = JOptionPane.showOptionDialog(null, panel,
                                                  "Password for keys",
                                                  JOptionPane.NO_OPTION,
                                                  JOptionPane.PLAIN_MESSAGE,
                                                  null, options, options[0]);

        if (option != 0) // Not pressing OK button
            return null;

        String p1 = new String(pass.getPassword());
        if (!encrypting)
        {
            return hashPassword(p1);
        }

        label.setText("Re-type password:");
        pass.setText("");
        option = JOptionPane.showOptionDialog(null, panel, "Password for keys",
                                              JOptionPane.NO_OPTION,
                                              JOptionPane.PLAIN_MESSAGE, null,
                                              options, options[0]);
        if (option != 0)
            return null;
        String p2 = new String(pass.getPassword());
        if (!p1.equals(p2))
        {
            JOptionPane.showMessageDialog(null, "Passwords do not match",
                                          "ERROR", JOptionPane.ERROR_MESSAGE);
            return null;
        }
        return hashPassword(p1);
    }

    /**
     * Generate a salt value
     * @return SHA512 Hash for the salt
     */
    public String genSalt()
    {
        try
        {
            MessageDigest md = MessageDigest.getInstance("SHA-512");
            String seed = String.valueOf(Math.random() * Integer.MAX_VALUE);
            byte[] hashData = md.digest(seed.getBytes());
            BigInteger pass = new BigInteger(1, hashData);
            StringBuilder temp = new StringBuilder(pass.toString(16));
            while (temp.length() % 64 != 0)
                temp.insert(0, '0');
            return new String(temp);
        }
        catch (NoSuchAlgorithmException e1)
        {
            return null;
        }
    }

    /**
     * Given a list of keys, encrypt them
     * @param keys The keys to be encrypted
     * @return The keys encrypted and encoded in base64
     */
    private String encryptKeys(String[] keys)
    {
        String password = getKeysPassword(true);
        String salt = genSalt();
        if (password == null || salt == null)
            return null;

        String allKeys = salt;
        for (String k : keys)
            allKeys += "\n" + k;

        byte[] plainText = allKeys.getBytes();
        String[] encryptWith = new String[] { password };
        for (int x = 0; x < ROUNDS; x++)
            plainText = eea.encryptData(plainText, encryptWith);

        return new String(plainText);
    }

    /**
     * Given keys data, decrypt it and return the keys
     * @param keysData The keys data to be decrypted
     * @return The keys decrypted
     */
    private String[] decryptKeys(byte[] keysData)
    {
        String password = getKeysPassword(false);
        if (password == null)
            return null;

        String[] encryptWith = new String[] { password };
        for (int x = 0; x < ROUNDS; x++)
            keysData = eea.decryptData(keysData, encryptWith);

        String[] keys = new String(keysData).split("\n");
        if (!eea.validKeyCheck(keys))
        {
            JOptionPane.showMessageDialog(
                null,
                "Invalid keys detected.\nThis can be due to:\n"
                    + "  1. The password entered is incorrect\n"
                    + "  2. The keys file is corrupted\n",
                "ERROR", JOptionPane.ERROR_MESSAGE);
            return null;
        }

        return Arrays.copyOfRange(keys, 1, keys.length); // Remove salt
    }

    /**
     * Saves the the array of keys as a serialized object into keys.ser
     * @param keys Array of keys to save
     * @return Returns true if the operation was successful
     */
    public boolean writeKeys(String[] keys)
    {
        JFileChooser fc = new JFileChooser();
        FileFilter filter = new FileNameExtensionFilter("Keys File", "keys");
        fc.setFileFilter(filter);
        fc.setDialogTitle("Select where to save the keys file");
        fc.showSaveDialog(null);
        File keysFile = fc.getSelectedFile();
        try
        {
            if (!keysFile.getName().endsWith(".keys"))
                keysFile = new File(keysFile.getAbsolutePath() + ".keys");
            if (keysFile.exists())
            {
                int x = JOptionPane.showConfirmDialog(
                    null,
                    "The keys file you entered already exists.\n"
                        + "Are you sure you would like to continue?",
                    "Caution", JOptionPane.YES_OPTION);
                if (x != JOptionPane.YES_OPTION)
                    return false;
            }

            FileWriter writer = new FileWriter(keysFile);
            String allKeys = encryptKeys(keys);
            if (allKeys == null)
            {
                writer.close();
                return false;
            }
            writer.write(allKeys);
            writer.close();
        }
        catch (FileNotFoundException e1)
        {
            e1.printStackTrace();
            return false;
        }
        catch (IOException e1)
        {
            e1.printStackTrace();
            return false;
        }
        catch (NullPointerException e)
        {
            System.out.println("Keys file save aborted");
            return false;
        }
        return true;
    }

    /**
     * Reads in a serialized array of keys from keys.ser
     * @return Array of keys
     */
    private String[] readKeys()
    {
        String[] keys = null;
        // Legacy
        if (keysExist())
        {
            try
            {
                ObjectInputStream objInputStream = new ObjectInputStream(
                    new FileInputStream(legacyKeyFilePath));
                keys = (String[]) objInputStream.readObject();
                objInputStream.close();
            }
            catch (IOException e1)
            {
                e1.printStackTrace();
            }
            catch (ClassNotFoundException e1)
            {
                e1.printStackTrace();
            }
            return keys;
        }

        JFileChooser fc = new JFileChooser();
        FileFilter filter = new FileNameExtensionFilter("Keys File", "keys");
        fc.setFileFilter(filter);
        fc.setDialogTitle("Select the keys file to use");
        fc.showOpenDialog(null);
        File keysFile = fc.getSelectedFile();
        try
        {
            if (!keysFile.getName().endsWith(".keys"))
            {
                JOptionPane
                    .showMessageDialog(null,
                                       "The file selected is not a keys file.",
                                       "ERROR", JOptionPane.ERROR_MESSAGE);
                return keys;
            }
            byte[] data = Files.readAllBytes(
                Paths.get(keysFile.getAbsolutePath()));
            keys = decryptKeys(data);
        }
        catch (IOException e1)
        {
            e1.printStackTrace();
        }
        catch (IllegalArgumentException e)
        {
            e.printStackTrace();
        }
        catch (NullPointerException e)
        {
            System.out.println("Keys file selection aborted");
        }
        return keys;
    }

    public void viewKeys()
    {
        String[] keys = readKeys();
        if (keys == null)
            return;

        JTextArea textArea = new JTextArea();
        textArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
        textArea.setLineWrap(true);
        textArea.setWrapStyleWord(true);
        textArea.setEditable(false);

        JScrollPane scrollPane = new JScrollPane();
        scrollPane.setVerticalScrollBarPolicy(
            JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);

        String keysList = new String();
        for (int x = 0; x < keys.length; x++)
        {
            keysList += String.valueOf(x + 1) + ". " + keys[x];
            if (x != keys.length - 1)
                keysList += "\n";
        }
        textArea.setText(keysList);
        scrollPane.setViewportView(textArea);
        scrollPane.setPreferredSize(new Dimension(500, 350));
        JOptionPane.showMessageDialog(null, scrollPane, "Keys",
                                      JOptionPane.DEFAULT_OPTION);
    }

    /**
     * Checks if the keys.ser file exits
     * @return If the keys.ser file exits
     */
    public boolean keysExist()
    {
        File file = new File(legacyKeyFilePath);
        return file.exists();
    }

    /**
     * If the keys.ser file exists, it reads them in and returns them
     * otherwise it returns null
     * @return Array of keys, or null if no key file exits
     */
    public String[] getKeys()
    {
        return readKeys();
    }

    /**
     * Deletes the keys.ser file containing the keys
     * @return If the deletion was successful
     */
    public boolean deleteKeys()
    {
        // Legacy
        File file = new File(legacyKeyFilePath);
        if (file.exists())
            return file.delete();

        JFileChooser fc = new JFileChooser();
        FileFilter filter = new FileNameExtensionFilter("Keys File", "keys");
        fc.setFileFilter(filter);
        fc.setDialogTitle("Select the keys file to delete");
        fc.showDialog(null, "Delete");
        File keysFile = fc.getSelectedFile();
        try
        {
            if (!keysFile.getName().endsWith(".keys"))
            {
                JOptionPane
                    .showMessageDialog(null,
                                       "The file selected is not a keys file.",
                                       "ERROR", JOptionPane.ERROR_MESSAGE);
                return false;
            }
            int x = JOptionPane.showConfirmDialog(
                null, "Are you sure you want to delete this keys file?",
                "Caution", JOptionPane.YES_OPTION);
            if (x != JOptionPane.YES_OPTION)
                return false;
            return keysFile.delete();
        }
        catch (NullPointerException e)
        {
            System.out.println("Keys file selection aborted");
        }
        return false;
    }
}

import java.awt.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

public class SystemControl1 {

    public static String performAction(String action) {
        switch (action) {
            case "showDesktop":
                return showDesktop();
            case "openStartMenu":
                return openStartMenu();
            case "lockScreen":
                return lockScreen();
            case "openFileExplorer":
                return openFileExplorer();
            case "openRunDialog":
                return openRunDialog();
            case "openTaskManager":
                return openTaskManager();
            case "takeScreenshot":
                return takeScreenshot();
            case "shutdown":
                return shutdown();
            case "restart":
                return restart();
            case "openNotepad":
                return openNotepad();
            case "openCalculator":
                return openCalculator();
            case "openGmail":
                return openGmail();
            case "openSpotify":
                return openSpotify();
            case "openInstagram":
                return openInstagram();
            case "openFacebook":
                return openFacebook();
            case "search":
                return searchGoogle();
            case "openSettings":
                return openSettings();
            default:
                return "Unknown action";
        }
    }

    private static String showDesktop() {
        return pressKeys(KeyEvent.VK_WINDOWS, KeyEvent.VK_D);
    }

    private static String openStartMenu() {
        return pressKey(KeyEvent.VK_WINDOWS);
    }

    private static String lockScreen() {
        try {
            // Execute the system command to lock the screen
            Runtime.getRuntime().exec("rundll32 user32.dll,LockWorkStation");
            return "Lock screen command executed.";
        } catch (IOException e) {
            return "Error executing lock screen command.";
        }
    }

    private static String openFileExplorer() {
        return executeCommand("explorer");
    }

    private static String openRunDialog() {
        return executeCommand("cmd /c start");
    }

    private static String openTaskManager() {
        return pressKeys(KeyEvent.VK_CONTROL, KeyEvent.VK_SHIFT, KeyEvent.VK_ESCAPE);
    }

    private static String takeScreenshot() {
        return pressKeys(KeyEvent.VK_WINDOWS, KeyEvent.VK_PRINTSCREEN);
    }

    private static String shutdown() {
        return executeCommand("shutdown -s -t 0");
    }

    private static String restart() {
        return executeCommand("shutdown -r -t 0");
    }

    private static String openNotepad() {
        return executeCommand("notepad");
    }

    private static String openCalculator() {
        return executeCommand("calc");
    }

    private static String openGmail() {
        return executeCommand("cmd /c start https://mail.google.com");
    }

    private static String openSpotify() {
        return executeCommand("cmd /c start https://open.spotify.com");
    }

    private static String openInstagram() {
        return executeCommand("cmd /c start https://www.instagram.com");
    }

    private static String openFacebook() {
        return executeCommand("cmd /c start https://www.facebook.com");
    }
    private static String searchGoogle() {
        return executeCommand("cmd /c start https://www.google.com/search?q=your_query_here");
    }

    private static String openSettings() {
        return executeCommand("cmd /c start ms-settings:");
    }

    private static String pressKey(int key) {
        try {
            Robot robot = new Robot();
            robot.keyPress(key);
            robot.keyRelease(key);
            return "Command executed.";
        } catch (AWTException e) {
            return "Error executing command.";
        }
    }

    private static String pressKeys(int... keys) {
        try {
            Robot robot = new Robot();
            for (int key : keys) robot.keyPress(key);
            for (int key : keys) robot.keyRelease(key);
            return "Command executed.";
        } catch (AWTException e) {
            return "Error executing command.";
        }
    }

    private static String executeCommand(String command) {
        try {
            Runtime.getRuntime().exec(command);
            return "Command executed.";
        } catch (IOException e) {
            return "Error executing command.";
        }
    }
}

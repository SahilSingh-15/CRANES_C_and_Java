import java.io.*;
import java.net.*;

public class SimpleHttpServer1 {
    public static void main(String[] args) throws IOException {
        int port = 8000;
        ServerSocket serverSocket = new ServerSocket(port);
        System.out.println("Server running on port " + port);

        while (true) {
            Socket clientSocket = serverSocket.accept();
            handleRequest(clientSocket);
        }
    }

    private static void handleRequest(Socket clientSocket) throws IOException {
        BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);

        String requestLine = in.readLine();
        System.out.println("Request: " + requestLine);
        
        String action = getActionFromRequest(requestLine);

        String response = SystemControl1.performAction(action);
        out.println(response);

        in.close();
        out.close();
        clientSocket.close();
    }

    private static String getActionFromRequest(String requestLine) {
        if (requestLine == null || !requestLine.startsWith("GET /")) return "";

        String[] parts = requestLine.split(" ");
        String path = parts[1];

        return path.substring(1);
    }
}

using System;
using System.IO;
using UnityEngine;
using System.Threading;
using System.Net.Sockets;





	public class WIFIConnectionScript : MonoBehaviour
	{
		public static float J1X;
		public static float J1Y;
		public static float J2X;
		public static float J2Y;
		public static bool Jump;
		public static bool Shoot;

		public static float CAMX;
		public static float CAMY;




    public bool connect;
		public int port = 80;   //Puerto
		public string host = "192.175.5.1";
		public string recievedData; //string para recibir información

		public delegate void WIFIEvents(string incomigString);
		public static event WIFIEvents WhenReceiveDataCall;
		private bool abort; //Variable para saber si abortar o no el hilo
		private static bool socketReady = false; //Si el socket está listo

		private TcpClient tcpClient; //Crea un socket	
		private Thread secondaryThread; //Hilo para recibir información
		private static StreamWriter streamWriter; //stream de escritura
		private static StreamReader streamReader; //stream de lectura
		private NetworkStream networkStream; //stream de red
		private SynchronizationContext mainThread;  //6

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.K))
        {
            Debug.Log("K presionada");

            WIFIConnectionScript.SendData(
                "M1=100;M2=100;M3=100;M4=100"
            );
        }
    }
    void Start()
		{
			if (connect)
			{
				SetupSocket();
			}
		}

		public void SetupSocket()
		{
			try
			{
				tcpClient = new TcpClient(host, port); //Se conecta al Arduino en el puerto indicado
				networkStream = tcpClient.GetStream(); //crea un stream para enviar y recibir información
				streamWriter = new StreamWriter(networkStream); //crea stream de escritura
				streamReader = new StreamReader(networkStream); //crea stream de lectura
				socketReady = true; //Si todo funciona bien el socket está listo
				CheckingMainThread();
				secondaryThread = new Thread(RecieveData);
				secondaryThread.Start();
				print("Conectado Correctamente");
			}
			catch (Exception e)
			{
				print("Error al intentar conectarse"); //Si hay error en el socket            
			}
		}

		private void CheckingMainThread()
		{
			mainThread = SynchronizationContext.Current;  //6

			if (mainThread == null)  //6
			{
				mainThread = new SynchronizationContext();  //6
			}
		}

		void RecieveData()
		{
			while (true)
			{
				if (abort)
				{
					secondaryThread.Abort();
					break;
				}

				if (!socketReady)
				{
					return;
				}

				if (networkStream.DataAvailable)
				{
					recievedData = streamReader.ReadLine();

                string[] datos = recievedData.Split(',');

                if (datos.Length >= 6)
                {
                    Jump = datos[0] == "1";
                    Shoot = datos[1] == "1";

                    float.TryParse(datos[2], out J1X);
                    float.TryParse(datos[3], out J1Y);

                    float.TryParse(datos[4], out J2X);
                    float.TryParse(datos[5], out J2Y);
                }
                Debug.Log(
 "Jump=" + Jump +
 " Shoot=" + Shoot +
 " J1X=" + J1X +
 " J1Y=" + J1Y +
 " J2X=" + J2X +
 " J2Y=" + J2Y
);

                mainThread.Send((object state) =>
					{
						if (WhenReceiveDataCall != null)
						{
							WhenReceiveDataCall(recievedData);
						}
					}, null);
				}

				Thread.Sleep(1);
			}
		}

		public static void SendData(string infoToSend)
		{
			if (!socketReady)
			{
				return;
			}

			streamWriter.Write(infoToSend + '\n'); //Envía la información usando el stream
			streamWriter.Flush(); //Limpia el stream
		}

		public void CloseSocket() //Cierra el socket
		{
			if (!socketReady)
			{
				return;
			}

			abort = true;
			socketReady = false;
			tcpClient.Close();
			streamWriter.Close();
			streamReader.Close();
		}

		private void OnDestroy() //Cuando se destruye el objeto
		{
			CloseSocket();
		}

		private void OnApplicationQuit() //Cuando se sale de la app
		{
			CloseSocket();
		}
	}

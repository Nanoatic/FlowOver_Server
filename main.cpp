
#include "includes.h"
#include "computing_client.h"
#include "global_processiong_vars.h"

typedef std::chrono::duration<float> float_seconds;


sockaddr_in server_addr, client_addr;
socklen_t clientAddrSize;

GtkWidget *window;
GtkWidget * image;
GtkWidget * image2;
GtkWidget * box_for_image;
GtkWidget * box_for_buttons;
GtkWidget * box_global;
GtkWidget * button_upload;
GtkWidget * button_normal_inv;
GtkWidget * button_distributed_inv;

void server_shutdown(){
    puts("closing sockets \n");
    for (auto i = 0; i < WAI_FOR_CLIENTS; ++i) {
        if (server_things::available_slots[i] == false) {
            close(server_things::clients[i]->client_socket_);
            shutdown(server_things::clients[i]->client_socket_, SHUT_RDWR);

        }
    }
    server_things::connected_clients =0;
    puts("sockets closed\n");
    server_things::running = false;
    global_processiong_vars::multiplication_message_queue.clear();
    shutdown(server_things::server_socket, SHUT_RDWR);
    close(server_things::server_socket);
}

int server_init() {




    //Server socket parameters
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    //Init Socket
    server_things::server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
//char yes='1'; // Solaris people use this

// lose the pesky "Address already in use" error message
    if (setsockopt(server_things::server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    }
    //bind sockt to address
    if (bind(server_things::server_socket, (sockaddr *) &server_addr, sizeof(sockaddr_in)) < 0) {
        std::cout << "error while binding\n";
        return -1;
    }
    listen(server_things::server_socket, 10);
    return 0;
}
void wait_for_clients(int n) {
    while (server_things::connected_clients < n);
}

int find_available_slot() {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (server_things::available_slots[i] == true)
            return i;
    }
    return -1;
}

void init_all_slots() {
    for (auto &available_slot : server_things::available_slots) {
        available_slot = true;

    }
}
void *start_server(void *) {
    //Server Init Begin
    struct sigaction sigIntHandler{};
    sigIntHandler.sa_handler = interrupt_exception::sig_to_exception;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);
    sigaction(SIGKILL, &sigIntHandler, nullptr);
    sigaction(SIGTERM, &sigIntHandler, nullptr);
    sigaction(SIGTERM, &sigIntHandler, nullptr);
    init_all_slots();
    if (server_init() < 0) exit(EXIT_FAILURE);
    signal(SIGPIPE, SIG_IGN);
    //Server Init End


    helper_class::display_info_syncronized_ln(

            "******** Server started ********\n"
                    "*                              *\n"
                    "*   Dispatching Operations ... *\n"
                    "*   by Alaeddine Frad          *\n"
                    "*                              *\n"
                    "********************************\n"
                    "Server started accepting ... \n");

    server_things::running= true;
    try {


        while (server_things::running) {

            int index = find_available_slot();
            /*   for (int i = 0; i <MAX_CLIENTS ; ++i) {
                   std::cout<<i<<":"<<main_processing::available_slots[i]<<std::endl;
               }*/
            // std::cout<<index <<std::endl;
            if (index == -1) {
                puts("Client tried to connect but server can't accept more clients \n");
                continue;
            }

            clientAddrSize = sizeof(sockaddr_in);
            int client_socket = accept(server_things::server_socket, (sockaddr *) &client_addr, &clientAddrSize);

            if (client_socket < 0) {
                return nullptr;
            }
            if (!server_things::running) {
                return nullptr;
            }
            auto client = new computing_client(index, client_socket, client_addr);
            client->start_client();
            server_things::available_slots[index] = false;
            server_things::clients[index] = client;


        }

    } catch (interrupt_exception &e) {
        server_shutdown();
    }
    return nullptr;
}
static mat<double> image_to_mat(GtkImage * image){

    GdkPixbuf * pixbuf = gtk_image_get_pixbuf(image);
    mat<double> matres(gdk_pixbuf_get_height(pixbuf) ,
               gdk_pixbuf_get_width(pixbuf) *gdk_pixbuf_get_n_channels(pixbuf)  );
    guchar * pixels = gdk_pixbuf_get_pixels(pixbuf);
    for (int i = 0; i < matres.getRows() ; ++i) {
        for (int j = 0; j <matres.getCols() ; ++j) {
            matres.at(i,j)= pixels[i*matres.getCols()+ j]/MAX_COLOR;
        }
    }
    return  matres;

}
static void  mat_to_image_cpy(mat<double> matin ,GtkImage * image ,GtkImage * distimage){
    GdkPixbuf * pixbuf = gtk_image_get_pixbuf(image);

    GdkPixbuf * newpixbuf = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(pixbuf),
                                           gdk_pixbuf_get_has_alpha(pixbuf),
                                           gdk_pixbuf_get_bits_per_sample(pixbuf),
                                           gdk_pixbuf_get_width(pixbuf),
                                           gdk_pixbuf_get_height(pixbuf));


    guchar * pixels = gdk_pixbuf_get_pixels(newpixbuf);
    for (int i = 0; i < matin.getRows() ; ++i) {
        for (int j = 0; j <matin.getCols() ; ++j) {
            pixels[i*matin.getCols()+ j] = (guchar) (abs(matin.at(i,j))*MAX_COLOR);
        }
    }
    gtk_image_set_from_pixbuf(distimage,newpixbuf);

}

static void
press_upload (GtkWidget *widget,
             gpointer   data)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new ("Open File",
                                          GTK_WINDOW(window),
                                          action,
                                          ("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          ("_Open"),
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);
        gtk_image_set_from_file(GTK_IMAGE(image),filename);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);

}
static void
press_normal_inv(GtkWidget *widget,
                     gpointer   data)
{
    GtkWidget *dialog, *label, *content_area;
    GtkDialogFlags flags;


    dialog = gtk_dialog_new_with_buttons ("Processing speed",
                                          GTK_WINDOW(window),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                          ("_OK"),
                                          GTK_RESPONSE_NONE,
                                          NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    label = gtk_label_new ("processing ...");

    // Ensure that the dialog box is destroyed when the user responds

    g_signal_connect_swapped (dialog,
                              "response",
                              G_CALLBACK (gtk_widget_destroy),
                              dialog);

    // Add the label, and show everything we’ve added

    gtk_container_add (GTK_CONTAINER (content_area), label);
    gtk_widget_hide_on_delete(dialog);
    gtk_window_set_modal(GTK_WINDOW(dialog),TRUE);
    gtk_widget_show_all(dialog);

    high_resolution_clock::time_point tn1 = high_resolution_clock::now();
    mat matconv = image_to_mat(GTK_IMAGE(image));
    bool is_left;
    mat matinv =matconv.inverse_nm_with_lu(is_left);
    if( matconv.getSize()!=0){
        mat_to_image_cpy(matinv,GTK_IMAGE(image),GTK_IMAGE(image2));


    } else {
        g_printerr("can t find inverse");
    }
    high_resolution_clock::time_point tn2 = high_resolution_clock::now();
    string pstr = "normal process finished : " +to_string( duration_cast<float_seconds>( tn2 - tn1 ).count()  )+" s";
    gtk_label_set_label(GTK_LABEL(label),pstr.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));




}
static void
press_distributed_inv(GtkWidget *widget,
                     gpointer   data)
{
    GtkWidget *dialog, *label, *content_area;
    GtkDialogFlags flags;


    dialog = gtk_dialog_new_with_buttons ("Processing speed",
                                          GTK_WINDOW(window),
                                          GTK_DIALOG_DESTROY_WITH_PARENT,
                                          ("_OK"),
                                          GTK_RESPONSE_NONE,
                                          NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    label = gtk_label_new ("processing ...");

    // Ensure that the dialog box is destroyed when the user responds

    g_signal_connect_swapped (dialog,
                              "response",
                              G_CALLBACK (gtk_widget_destroy),
                              dialog);

    // Add the label, and show everything we’ve added

    gtk_container_add (GTK_CONTAINER (content_area), label);
    gtk_widget_hide_on_delete(dialog);
    gtk_window_set_modal(GTK_WINDOW(dialog),TRUE);
    gtk_widget_show_all(dialog);



    high_resolution_clock::time_point tn1 = high_resolution_clock::now();
    mat matconv = image_to_mat(GTK_IMAGE(image));
    bool is_left;
    byte tag = 0x01;
    mat matinv =matconv.inverse_nm_with_lu_distributed(is_left,tag,server_things::available_slots,
                                                       server_things::clients,
                                                       global_processiong_vars::multiplication_message_queue);
    if( matconv.getSize()!=0){
        mat_to_image_cpy(matinv,GTK_IMAGE(image),GTK_IMAGE(image2));


    } else {
        g_printerr("can t find inverse");
    }
    high_resolution_clock::time_point tn2 = high_resolution_clock::now();
    string pstr = "normal process finished : " +to_string( duration_cast<float_seconds>( tn2 - tn1 ).count()  )+" s";
    gtk_label_set_label(GTK_LABEL(label),pstr.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));





}

static GtkWidget*
create_image (string name)
{
  GtkWidget *image;


  image = gtk_image_new_from_file (name.c_str());

  return image;
}
static void
activate (GtkApplication* app,
          gpointer        user_data)
{

    button_upload = gtk_button_new_with_label("Select image ...");
    button_normal_inv = gtk_button_new_with_label("Inverse normal");
    button_distributed_inv = gtk_button_new_with_label("Inverse distributed");

    image = create_image("/home/alaeddine/CLionProjects/FlowOver_Server/image1.jpg");
    image2 = create_image("/home/alaeddine/CLionProjects/FlowOver_Server/image2.pn");


    box_for_image = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,3);
    box_for_buttons = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    box_global = gtk_box_new(GTK_ORIENTATION_VERTICAL,3);


    //images
    gtk_box_pack_start(GTK_BOX(box_for_image),image,TRUE,TRUE,20);
    gtk_box_pack_start(GTK_BOX(box_for_image),image2,TRUE,TRUE,20);

    //buttons
    gtk_container_add(GTK_CONTAINER(box_for_buttons),button_upload);
    gtk_container_add(GTK_CONTAINER(box_for_buttons),button_normal_inv);
    gtk_container_add(GTK_CONTAINER(box_for_buttons),button_distributed_inv);


    gtk_box_pack_start(GTK_BOX(box_global),box_for_image,TRUE,TRUE,20);
    gtk_box_pack_start(GTK_BOX(box_global),box_for_buttons,TRUE,TRUE,20);

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "FlowOver Project");
    gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);


    gtk_container_add(GTK_CONTAINER(window),box_global);

    g_signal_connect(button_upload,"clicked",G_CALLBACK(press_upload),NULL);
    g_signal_connect(button_normal_inv,"clicked",G_CALLBACK(press_normal_inv),NULL);
    g_signal_connect(button_distributed_inv,"clicked",G_CALLBACK(press_distributed_inv),NULL);



    gtk_widget_show_all (window);
}









int main(int argc , char ** argv) {
#ifdef FILE_QUEUE
    database_singleton::init();
#endif

    thread thready = thread(&start_server, nullptr);
    helper_class::display_info_syncronized_ln("waiting for clients ...");
    wait_for_clients(WAI_FOR_CLIENTS);

    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    server_shutdown();
    thready.join();
    return status;

}
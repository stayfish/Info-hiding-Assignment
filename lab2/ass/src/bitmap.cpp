#include"bitmap.h"


// todo: 涉及大小端序的问题，需要解决
int BYTE_to_int(const BYTE* arr, int n) {
    int data = 0;
    for(int i = 0; i < n; i++) {
        data = data + (arr[i] << (8 * i));
    }
    return data;
}

void copy_BYTE(BYTE* byte1, const BYTE* byte2, int n) {
    for(int i = 0; i < n; i++) {
        byte1[i] = byte2[i];
    }
}

void BMP::write_BYTE_to(ostream& out, BYTE* byte, int n) {
    for(int i = 0; i < n; i++) {
        out << byte[i];
    }
}

int BMP::get_row_size() const {
    return 
    ((BYTE_to_int(m_info_header.depth, 2) * BYTE_to_int(m_info_header.width, 4)+31) >> 5) << 2;
}

int BMP::get_padding() {
    return 
    (4 - (BYTE_to_int(m_info_header.depth, 2) * BYTE_to_int(m_info_header.width, 4) >> 3) & 3);
}

int BMP::get_palette_colors() {
    return pow(2, BYTE_to_int(this->m_info_header.depth, 2));
}

BMP::~BMP() {
    if(m_palette)
        delete[] m_palette;
    if(m_bitmap)
        delete[] m_bitmap;
}

BMP::BMP(const BMP& bmp) {
    size_t size = sizeof(BMP);
    memcpy(this, &bmp, size);
    // copy PALETTE
    // m_colors = bmp.m_colors;
    this->m_palette = new RGBQUAD[m_colors];
    for(int i = 0; i < m_colors; i++) {
        this->m_palette[i].blue = bmp.m_palette[i].blue;
        this->m_palette[i].green = bmp.m_palette[i].green;
        this->m_palette[i].red = bmp.m_palette[i].red;
        this->m_palette[i].reserved = bmp.m_palette[i].reserved;
    }
    // copy BITMAP
    // m_row_size = bmp.m_row_size;
    m_bitmap_size = m_row_size * BYTE_to_int(this->m_info_header.height, 4);
    this->m_bitmap = new BYTE[m_bitmap_size];
    for(int i = 0; i < m_bitmap_size; i++) {
        this->m_bitmap[i] = bmp.m_bitmap[i];
    }
}

void BMP::operator=(const BMP& bmp) {
    // copy HEADER
    copy_BYTE(this->m_header.type, bmp.m_header.type, 2);
    copy_BYTE(this->m_header.bmp_size, bmp.m_header.bmp_size, 4);
    copy_BYTE(this->m_header.reserved1, bmp.m_header.reserved1, 2);
    copy_BYTE(this->m_header.reserved2, bmp.m_header.reserved2, 2);
    copy_BYTE(this->m_header.offset, bmp.m_header.offset, 4);
    // copy BITMAPINFOHEADER
    copy_BYTE(this->m_info_header.info_header_size, bmp.m_info_header.info_header_size, 4);
    copy_BYTE(this->m_info_header.width, bmp.m_info_header.width, 4);
    copy_BYTE(this->m_info_header.height, bmp.m_info_header.height, 4);
    copy_BYTE(this->m_info_header.planes, bmp.m_info_header.planes, 2);
    copy_BYTE(this->m_info_header.depth, bmp.m_info_header.depth, 2);
    copy_BYTE(this->m_info_header.method, bmp.m_info_header.method, 4);
    copy_BYTE(this->m_info_header.image_size, bmp.m_info_header.image_size, 4);
    copy_BYTE(this->m_info_header.h_resolution, bmp.m_info_header.h_resolution, 4);
    copy_BYTE(this->m_info_header.v_resolution, bmp.m_info_header.v_resolution, 4);
    copy_BYTE(this->m_info_header.h_resolution, bmp.m_info_header.h_resolution, 4);
    copy_BYTE(this->m_info_header.palette_colors, bmp.m_info_header.palette_colors, 4);
    copy_BYTE(this->m_info_header.key_colors, bmp.m_info_header.key_colors, 4);
    // copy PALETTE
    m_colors = bmp.m_colors;
    this->m_palette = new RGBQUAD[m_colors];
    for(int i = 0; i < m_colors; i++) {
        this->m_palette[i].blue = bmp.m_palette[i].blue;
        this->m_palette[i].green = bmp.m_palette[i].green;
        this->m_palette[i].red = bmp.m_palette[i].red;
        this->m_palette[i].reserved = bmp.m_palette[i].reserved;
    }
    // copy BITMAP
    m_row_size = bmp.m_row_size;
    m_bitmap_size = m_row_size * BYTE_to_int(this->m_info_header.height, 4);
    this->m_bitmap = new BYTE[m_bitmap_size];
    for(int i = 0; i < m_bitmap_size; i++) {
        this->m_bitmap[i] = bmp.m_bitmap[i];
    }
}

BMP::BMP(BMP&& bmp) {
    size_t size = sizeof(BMP);
    memcpy(this, &bmp, size);
    bmp.m_palette = nullptr;
    bmp.m_bitmap = nullptr;
}

void BMP::operator=(BMP&& bmp) {
    size_t size = sizeof(BMP);
    memcpy(this, &bmp, size);
    bmp.m_palette = nullptr;
    bmp.m_bitmap = nullptr;
    // // copy HEADER
    // copy_BYTE(this->m_header.type, bmp.m_header.type, 2);
    // copy_BYTE(this->m_header.bmp_size, bmp.m_header.bmp_size, 4);
    // copy_BYTE(this->m_header.reserved1, bmp.m_header.reserved1, 2);
    // copy_BYTE(this->m_header.reserved2, bmp.m_header.reserved2, 2);
    // copy_BYTE(this->m_header.offset, bmp.m_header.offset, 4);
    // // copy BITMAPINFOHEADER
    // copy_BYTE(this->m_info_header.info_header_size, bmp.m_info_header.info_header_size, 4);
    // copy_BYTE(this->m_info_header.width, bmp.m_info_header.width, 4);
    // copy_BYTE(this->m_info_header.height, bmp.m_info_header.height, 4);
    // copy_BYTE(this->m_info_header.planes, bmp.m_info_header.planes, 2);
    // copy_BYTE(this->m_info_header.depth, bmp.m_info_header.depth, 2);
    // copy_BYTE(this->m_info_header.method, bmp.m_info_header.method, 4);
    // copy_BYTE(this->m_info_header.image_size, bmp.m_info_header.image_size, 4);
    // copy_BYTE(this->m_info_header.h_resolution, bmp.m_info_header.h_resolution, 4);
    // copy_BYTE(this->m_info_header.v_resolution, bmp.m_info_header.v_resolution, 4);
    // copy_BYTE(this->m_info_header.h_resolution, bmp.m_info_header.h_resolution, 4);
    // copy_BYTE(this->m_info_header.palette_colors, bmp.m_info_header.palette_colors, 4);
    // copy_BYTE(this->m_info_header.key_colors, bmp.m_info_header.key_colors, 4);
    // // copy PALETTE
    // m_colors = bmp.m_colors;
    // this->m_palette = bmp.m_palette;
    // // copy BITMAP
    // m_row_size = bmp.m_row_size;
    // m_bitmap_size = m_row_size * BYTE_to_int(this->m_info_header.height, 4);
    // this->m_bitmap = bmp.m_bitmap;
    // // 交出所有权
    // bmp.m_palette = nullptr;
    // bmp.m_bitmap = nullptr;
}

void BMP::load_bmp(const string& path) {
    ifstream bmp(path, ios::in | ios::binary);
    size_t header_offset = sizeof(int) * 3;
    char* start = (char*)this + header_offset;
    bmp.read((char*)start, sizeof(HEADER) + sizeof(BITMAPINFOHEADER));
    // // read HEADER
    // bmp.read(reinterpret_cast<char*>(m_header.type), 2);
    // bmp.read(reinterpret_cast<char*>(m_header.bmp_size), 4);
    // bmp.read(reinterpret_cast<char*>(m_header.reserved1), 2);
    // bmp.read(reinterpret_cast<char*>(m_header.reserved2), 2);
    // bmp.read(reinterpret_cast<char*>(m_header.offset), 4);
    // // read BITMAPINFOHEADER
    // bmp.read(reinterpret_cast<char*>(m_info_header.info_header_size), 4);
    // bmp.read(reinterpret_cast<char*>(m_info_header.width), 4);
    // bmp.read(reinterpret_cast<char*>(m_info_header.height), 4);
    // bmp.read(reinterpret_cast<char*>(m_info_header.planes), 2);
    // bmp.read(reinterpret_cast<char*>(m_info_header.depth), 2);
    // bmp.read(reinterpret_cast<char*>(m_info_header.method), 4);
    // bmp.read(reinterpret_cast<char*>(m_info_header.image_size), 4);
    // bmp.read(reinterpret_cast<char*>(m_info_header.h_resolution), 4);
    // bmp.read(reinterpret_cast<char*>(m_info_header.v_resolution), 4);
    // bmp.read(reinterpret_cast<char*>(m_info_header.palette_colors), 4);
    // bmp.read(reinterpret_cast<char*>(m_info_header.key_colors), 4);
    // read PALETTE
    m_colors = get_palette_colors();
    this->m_palette = new RGBQUAD[m_colors];
    for(int i = 0; i < m_colors; i++) {
        bmp.read(reinterpret_cast<char*>(&m_palette[i].blue), 1);
        bmp.read(reinterpret_cast<char*>(&m_palette[i].green), 1);
        bmp.read(reinterpret_cast<char*>(&m_palette[i].red), 1);
        bmp.read(reinterpret_cast<char*>(&m_palette[i].reserved), 1);
    }
    // read BITMAP
    m_row_size = get_row_size();
    m_bitmap_size = m_row_size * BYTE_to_int(this->m_info_header.height, 4);
    this->m_bitmap = new BYTE[m_bitmap_size];
    bmp.read(reinterpret_cast<char*>(m_bitmap), m_bitmap_size);
}

void BMP::write_bmp(const string& path) {
    ofstream bmp(path, ios::out | ios::binary);
    // write HEADER
    write_BYTE_to(bmp, m_header.type, 2);
    write_BYTE_to(bmp, m_header.bmp_size, 4);
    write_BYTE_to(bmp, m_header.reserved1, 2);
    write_BYTE_to(bmp, m_header.reserved1, 2);
    write_BYTE_to(bmp, m_header.offset, 4);
    // write BITMAPINFOHEADER
    write_BYTE_to(bmp, m_info_header.info_header_size, 4);
    write_BYTE_to(bmp, m_info_header.width, 4);
    write_BYTE_to(bmp, m_info_header.height, 4);
    write_BYTE_to(bmp, m_info_header.planes, 2);
    write_BYTE_to(bmp, m_info_header.depth, 2);
    write_BYTE_to(bmp, m_info_header.method, 4);
    write_BYTE_to(bmp, m_info_header.image_size, 4);
    write_BYTE_to(bmp, m_info_header.h_resolution, 4);
    write_BYTE_to(bmp, m_info_header.v_resolution, 4);
    write_BYTE_to(bmp, m_info_header.palette_colors, 4);
    write_BYTE_to(bmp, m_info_header.key_colors, 4);
    // write PALETTE
    for(int i = 0; i < m_colors; i++) {
        write_BYTE_to(bmp, &m_palette[i].blue, 1);
        write_BYTE_to(bmp, &m_palette[i].green, 1);
        write_BYTE_to(bmp, &m_palette[i].red, 1);
        write_BYTE_to(bmp, &m_palette[i].reserved, 1);
    }
    // write BITMAP
    write_BYTE_to(bmp, m_bitmap, m_bitmap_size);
}



void BMP::set_bitmap(const BITMAP& bitmap, int n) {
    if(m_bitmap_size != n)
        exit(1);
    for(int i=0; i < m_bitmap_size; i++) {
        this->m_bitmap[i] = bitmap[i];
    }
}

int BMP::get_bitmap_size() const {
    return m_bitmap_size;
}

BITMAP BMP::get_bitmap() const {
    BITMAP res = new BYTE[m_bitmap_size];
    for(int i = 0; i < m_bitmap_size; i++) {
        res[i] = this->m_bitmap[i];
    }
    return res;
}


#ifdef TEST_BMP
void BMP::bmp_test() {
    BMP bmp;
    string path = "../data/tj-logo.bmp";
    string path2 = "../build/logo-copy.BMP";
    bmp.load_bmp(path);
    bmp.output_BYTE();
    BMP bmp2(bmp);
    bmp2.write_bmp(path2);
}

void BMP::output_BYTE() {
    // output HEADER
    print_BYTE(m_header.type, 2);
    print_BYTE(m_header.bmp_size, 4);
    print_BYTE(m_header.reserved1, 2);
    print_BYTE(m_header.reserved1, 2);
    print_BYTE(m_header.offset, 2);
    // output BITMAPINFOHEADER
    print_BYTE(m_info_header.info_header_size, 4);
    print_BYTE(m_info_header.width, 4);
    print_BYTE(m_info_header.height, 4);
    print_BYTE(m_info_header.planes, 2);
    print_BYTE(m_info_header.depth, 2);
    print_BYTE(m_info_header.method, 4);
    print_BYTE(m_info_header.image_size, 4);
    print_BYTE(m_info_header.h_resolution, 4);
    print_BYTE(m_info_header.v_resolution, 4);
    print_BYTE(m_info_header.palette_colors, 4);
    print_BYTE(m_info_header.key_colors, 4);
    // output PALETTE
    for(int i = 0; i < m_colors; i++) {
        print_BYTE(&m_palette[i].blue, 1);
        print_BYTE(&m_palette[i].green, 1);
        print_BYTE(&m_palette[i].red, 1);
        print_BYTE(&m_palette[i].reserved, 1);
    }
    // output BITMAP
    print_BYTE(m_bitmap, m_bitmap_size);

    cout << '\n';
}

void print_BYTE(BYTE* byte, int n) {
    static int counts = 0;
    string path = "../build/lena_bit.txt";
    ofstream out(path, ios::out);

    if(counts == 0) {
        out << "00000000 ";
        cout << "00000000 ";
        for(int i = 0; i < 16; i++) {
            out << setiosflags(ios::uppercase) << hex << setw(2) << setfill('0') << i << ' ';
            cout << setiosflags(ios::uppercase) << hex << setw(2) << setfill('0') << i << ' ';
        }
    }
    
    for(int i = 0; i < n; i++) {

        if(counts % 16 == 0) {
            out << '\n' << setiosflags(ios::uppercase) << hex << setw(8) << setfill('0') <<(counts >> 4) << ' ';
            cout << '\n' << setiosflags(ios::uppercase) << hex << setw(8) << setfill('0') <<(counts >> 4) << ' ';
        }
        counts++;
        out << setiosflags(ios::uppercase) << hex << setw(2) << setfill('0') << int(byte[i]) << ' ';
        cout << setiosflags(ios::uppercase) << hex << setw(2) << setfill('0') << int(byte[i]) << ' ';
    }
}

#endif
int BMP::get_height() const {
    return BYTE_to_int(m_info_header.height, 4);
}

int BMP::get_width() const {
    return get_row_size();
}
#include <stdio.h>
#include <string.h>

#define MAX_ZONES 10
#define MAX_QRS 100
#define MAX_TICKETS_PER_ORDER 4

struct TicketZone {
    int zone_id;
    char zone_name[30];
    float ticket_price;
    int total_capacity;
    int sold_tickets;
};

struct QrCheckIn {
    char qr_code[30];
    int zone_id;
    int is_used;
};

int findZoneIndex(const struct TicketZone zones[], int total_zones, int zone_id);
float calculateTotalPrice(float ticket_price, int quantity, int is_early_bird);
int findQrIndex(const struct QrCheckIn qrs[], int total_qrs, const char qr_code[]);
int bookTickets(struct TicketZone zones[], int total_zones, int zone_id, int quantity, int is_early_bird, struct QrCheckIn qrs[], int total_qrs, int max_qrs);
void processCheckIn(struct QrCheckIn qrs[], int total_qrs, const char qr_code[]);
void displayZones(const struct TicketZone zones[], int total_zones);

int main() {
    struct TicketZone zones[MAX_ZONES] = {
        {1, "VIP", 2000000.0f, 5, 3},
        {2, "GA", 800000.0f, 100, 10},
        {3, "Zone A", 1200000.0f, 50, 0}
    };
    int total_zones = 3;

    struct QrCheckIn qrs[MAX_QRS];
    int total_qrs = 0;

    printf("=== HE THONG MAY TRAM BAN VE & SOAT VE LIVE CONCERT ===\n\n");
    displayZones(zones, total_zones);

    printf("\n>>> TEST CASE 1: DAT VE HOP LE (ZONE 3, 2 VE, EARLY BIRD) <<<\n");
    total_qrs = bookTickets(zones, total_zones, 3, 2, 1, qrs, total_qrs, MAX_QRS);

    printf("\n>>> TEST CASE 2: SO LUONG VE KHONG HOP LE (5 VE VA -1 VE) <<<\n");
    total_qrs = bookTickets(zones, total_zones, 2, 5, 0, qrs, total_qrs, MAX_QRS);
    total_qrs = bookTickets(zones, total_zones, 2, -1, 0, qrs, total_qrs, MAX_QRS);

    printf("\n>>> TEST CASE 3: MA KHU VUC KHONG TON TAI (ZONE 99) <<<\n");
    total_qrs = bookTickets(zones, total_zones, 99, 2, 0, qrs, total_qrs, MAX_QRS);

    printf("\n>>> TEST CASE 4: TRAN SUC CHUA KHU VUC (ZONE 1 CON 2 CHO, DAT 3 VE) <<<\n");
    total_qrs = bookTickets(zones, total_zones, 1, 3, 0, qrs, total_qrs, MAX_QRS);

    printf("\n>>> TEST CASE 5: CHECK-IN MA QR HOP LE LAN DAU <<<\n");
    processCheckIn(qrs, total_qrs, qrs[0].qr_code);

    printf("\n>>> TEST CASE 6: QUET LAI MA QR VUA CHECK-IN (DOUBLE CHECK-IN) <<<\n");
    processCheckIn(qrs, total_qrs, qrs[0].qr_code);

    printf("\n>>> TEST CASE 7: CHECK-IN MA QR KHONG TON TAI <<<\n");
    processCheckIn(qrs, total_qrs, "QR_UNKNOWN_999");

    printf("\n>>> TINH TRANG CAC KHU VUC VE SAU KHI XU LY <<<\n");
    displayZones(zones, total_zones);

    return 0;
}

int findZoneIndex(const struct TicketZone zones[], int total_zones, int zone_id) {
    if (total_zones <= 0) {
        return -1;
    }
    for (int i = 0; i < total_zones; i++) {
        if (zones[i].zone_id == zone_id) {
            return i;
        }
    }
    return -1;
}

float calculateTotalPrice(float ticket_price, int quantity, int is_early_bird) {
    float total = ticket_price * (float)quantity;
    if (is_early_bird == 1) {
        total = total * 0.85f;
    }
    return total;
}

int findQrIndex(const struct QrCheckIn qrs[], int total_qrs, const char qr_code[]) {
    if (total_qrs <= 0) {
        return -1;
    }
    for (int i = 0; i < total_qrs; i++) {
        if (strcmp(qrs[i].qr_code, qr_code) == 0) {
            return i;
        }
    }
    return -1;
}

int bookTickets(struct TicketZone zones[], int total_zones, int zone_id, int quantity, int is_early_bird, struct QrCheckIn qrs[], int total_qrs, int max_qrs) {
    if (quantity < 1 || quantity > MAX_TICKETS_PER_ORDER) {
        printf("[LOI]: So luong ve khong hop le (%d ve). Moi don chi duoc mua 1 den %d ve!\n",
               quantity, MAX_TICKETS_PER_ORDER);
        return total_qrs;
    }

    int zone_idx = findZoneIndex(zones, total_zones, zone_id);
    if (zone_idx == -1) {
        printf("[LOI]: Khong tim thay khu vuc co ma zone_id = %d tren he thong!\n", zone_id);
        return total_qrs;
    }

    if (zones[zone_idx].sold_tickets + quantity > zones[zone_idx].total_capacity) {
        printf("[LOI OVERBOOKING]: Khu vuc %s chi con %d cho trong, khong du de dat %d ve!\n",
               zones[zone_idx].zone_name,
               zones[zone_idx].total_capacity - zones[zone_idx].sold_tickets,
               quantity);
        return total_qrs;
    }

    if (total_qrs + quantity > max_qrs) {
        printf("[LOI]: He thong phat hanh ma QR da day!\n");
        return total_qrs;
    }

    float total_amount = calculateTotalPrice(zones[zone_idx].ticket_price, quantity, is_early_bird);
    zones[zone_idx].sold_tickets += quantity;

    printf("[DAT VE THANH CONG]: Khu vuc: %s | So luong: %d | Tong tien: %.0f VND (Early Bird: %s)\n",
           zones[zone_idx].zone_name,
           quantity,
           total_amount,
           is_early_bird == 1 ? "Co giam 15%" : "Khong");

    for (int i = 0; i < quantity; i++) {
        sprintf(qrs[total_qrs].qr_code, "QR_Z%d_%04d", zone_id, total_qrs + 1);
        qrs[total_qrs].zone_id = zone_id;
        qrs[total_qrs].is_used = 0;
        printf("  -> Phat hanh ma QR check-in: %s\n", qrs[total_qrs].qr_code);
        total_qrs++;
    }

    return total_qrs;
}

void processCheckIn(struct QrCheckIn qrs[], int total_qrs, const char qr_code[]) {
    if (total_qrs <= 0) {
        printf("[LOI]: He thong chua phat hanh ma QR nao de check-in!\n");
        return;
    }

    int qr_idx = findQrIndex(qrs, total_qrs, qr_code);
    if (qr_idx == -1) {
        printf("[LOI]: Ma QR '%s' khong ton tai tren he thong!\n", qr_code);
        return;
    }

    if (qrs[qr_idx].is_used == 1) {
        printf("[CANH BAO GIAN LAN]: Ma QR '%s' da duoc check-in truoc do! Tu choi vao cong!\n", qr_code);
        return;
    }

    qrs[qr_idx].is_used = 1;
    printf("[CHECK-IN THANH CONG]: Ma QR '%s' hop le! Moi quy khach vao khu vuc %d.\n",
           qr_code, qrs[qr_idx].zone_id);
}

void displayZones(const struct TicketZone zones[], int total_zones) {
    if (total_zones <= 0) {
        printf("[THONG BAO]: Danh sach khu vuc trong!\n");
        return;
    }

    printf("-----------------------------------------------------------------------------------------\n");
    printf("%-8s %-15s %-15s %-15s %-15s %-12s\n",
           "ZONE ID", "TEN KHU VUC", "GIA GOC(VND)", "TONG SUC CHUA", "DA BAN", "CON LAI");
    printf("-----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < total_zones; i++) {
        int remaining = zones[i].total_capacity - zones[i].sold_tickets;
        printf("%-8d %-15s %-15.0f %-15d %-15d %-12d\n",
               zones[i].zone_id,
               zones[i].zone_name,
               zones[i].ticket_price,
               zones[i].total_capacity,
               zones[i].sold_tickets,
               remaining);
    }
    printf("-----------------------------------------------------------------------------------------\n");
}

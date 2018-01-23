
void R16() {

    int x = 1;
    int y = 2;
    switch (x) {
        case 1:
            if (y >1 )
            {
                case 2:
                    x=1;
            }
            break;
        default:
            break;
    }

    switch ( y) {
        case 1:
            y=2;
            break;
        case 2:
            y = 3;
            {
                case 10:
                    y=10;
            }
            break;
        case 3:
            y = 4;
            break;
        default:
            y = 100;
            break;
    }
}




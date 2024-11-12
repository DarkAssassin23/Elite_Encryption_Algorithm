import EEAUtils

func main() async {
    loadConfig()
    let userIn = UserInput()
    while true {
        let ret = userIn.mainMenu()
        if ret == -1 {
            break
        } else if ret == 0 {
            print("Invalid selection.")
        } else {
            await userIn.submenu(menu: ret)
        }
    }
    print("goodbye.")
}

await main()

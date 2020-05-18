using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TitleUI : MonoBehaviour
{
    public GameObject registerWindow;
    public GameObject registerSuccessWindow;
    public GameObject registerFailedWindow;
    public GameObject loginFailedWindow;
    public GameObject loginDuplicatedWindow;

    // Start is called before the first frame update
    void Start()
    {
        registerWindow.SetActive(false);

        registerSuccessWindow.SetActive(false);
        registerFailedWindow.SetActive(false);
        loginFailedWindow.SetActive(false);
        loginDuplicatedWindow.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void OpenRegisterWindow()
    {
        registerWindow.SetActive(true);
    }

    public void CloseRegisterWindow()
    {
        registerWindow.SetActive(false);
    }

    public void CloseRegisterSuccessWindow()
    {
        registerSuccessWindow.SetActive(false);
    }

    public void CloseRegisterFailedWindow()
    {
        registerFailedWindow.SetActive(false);
    }

    public void CloseLoginFailedWindow()
    {
        loginFailedWindow.SetActive(false);
    }

    public void CloseLoginDuplicatedWindow()
    {
        loginDuplicatedWindow.SetActive(false);
    }
}
